#include "Player.h"
#include "GameManager.h"

Player::Player()
{
}

Player::Player(ColliderType colliderType, unsigned int projectionWidth, unsigned int projectionHeight):
	GameObject(colliderType)//Should this extend GameObject if doesn't have mesh?
{
	//BYTE VALUES MUST BE BETWEEN 0 AND 255
	//UNSIGNED SHORT VALUES MUST BE BETWEEN 0 AND 65535
	movementSpeed = 2; //The camera's movement speed
	mouseSensitivity = 400; //Mouse sensitivity, determines the camera's rotation speed

	health = 3;

	UpdateProjectionMatrix(projectionWidth,projectionHeight);

	playerHeight = 0.8f;

	transform.SetPosition(0, playerHeight, -5);
	transform.SetScale(0.5f, 1, 0.5f);

	isOnGameObject = false;
}

Player::~Player()
{
}

void Player::Update(float deltaTime)
{
	UpdateKeyInput(deltaTime);
	UpdateViewMatrix();

	float yPos = transform.GetPosition().y;

	if (yPos > playerHeight)
	{
		verticalSpeed -= 3 * deltaTime;
	}
	else if (yPos < playerHeight)
	{
		verticalSpeed = 0;
		XMFLOAT3 newPos = transform.GetPosition();
		newPos.y = 0.8f;
		transform.SetPosition(newPos);
	}

	//Update collider information
	//Collider probably won't be offset, so we won't even worry about that
	coll.center = transform.GetPosition();
	coll.dimensions = transform.GetScale();

	//transform.UpdatePhysics(deltaTime);
}

void Player::UpdateKeyInput(float deltaTime)
{
	float fwdSpeed = 0;//+ forward, - backward
	float sideSpeed = 0;//+ right, - left

	//Move forward on XZ plane when W pressed
	if (GetAsyncKeyState('W') & 0x8000) {
		fwdSpeed += movementSpeed * deltaTime;
	}
	//Move backward on XZ plane when S pressed
	if (GetAsyncKeyState('S') & 0x8000) {
		fwdSpeed -= movementSpeed * deltaTime;
	}
	//Move right relative to forward on XZ plane when D pressed
	if (GetAsyncKeyState('D') & 0x8000) {
		sideSpeed -= movementSpeed * deltaTime;
	}
	//Move left relative to forward on XZ plane when A pressed
	if (GetAsyncKeyState('A') & 0x8000) {
		sideSpeed += movementSpeed * deltaTime;
	}
	//Jump when spacebar pressed
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		Jump();
		jumpButtonHeld = true;
	}
	else {
		jumpButtonHeld = false;
	}

	//GAMEOBJECT COLLISIONS
	vector<GameObject>* goVector = GameManager::getInstance().GetGameObjectVector(); //Get the instance of the GameManager
	Collider nextFrameCollider = coll; //Make a collider to represent where the player will be on the next frame

	//Store the collider's next center point
	XMStoreFloat3(&nextFrameCollider.center,
		XMLoadFloat3(&transform.GetPosition()) + 
		XMLoadFloat3(&transform.GetForwardXZ()) * fwdSpeed + 
		XMLoadFloat3(&transform.GetRight()) * sideSpeed + 
		XMLoadFloat3(&transform.GetUp()) * (verticalSpeed * deltaTime));

	isOnGameObject = false; //Reset here

	//Start at 1, 0 is the ground
	for (byte i = 1; i < goVector->size(); i++)
	{
		//WITH PLAYER
		Collider* goCollider = (*goVector)[i].GetCollider(); //The GameObject's collider

		if (goCollider->collType == BOX && Collision::CheckCollisionBoxBox(goCollider, &nextFrameCollider))
		{
			if (coll.center.y - (coll.dimensions.y / 2) < goCollider->dimensions.y - .005f) //If the player isn't on top of the GameObject
			{
				//There shouldn't be any breakage due to that small offset value, but I'm leaving debug stuff here just in case
				//printf("NOT ON TOP %f, %f\n", coll.center.y - (coll.dimensions.y / 2), goCollider->dimensions.y);
				goto breakEnd;
			}
			else //If the player is on top of the GameObject
			{
				//printf("ON TOP %f, %f\n", coll.center.y - (coll.dimensions.y / 2), goCollider->dimensions.y);
				transform.SetPosition(transform.GetPosition().x, (float)(goCollider->dimensions.y + (coll.dimensions.y / 2)), transform.GetPosition().z); //Move the player to the top of the GameObject
				transform.MoveRelativeAxes(fwdSpeed, sideSpeed, 0);
				isOnGameObject = true;
				goto breakEnd;
			}
		}
		else if (goCollider->collType == SPHERE && Collision::CheckCollisionSphereBox(goCollider, &nextFrameCollider))
		{
			if (coll.center.y - (coll.dimensions.y / 2) < goCollider->dimensions.y - .005f) //If the player isn't on top of the GameObject
			{
				//There shouldn't be any breakage due to that small offset value, but I'm leaving debug stuff here just in case
				//printf("NOT ON TOP %f, %f\n", coll.center.y - (coll.dimensions.y / 2), goCollider->dimensions.y);
				goto breakEnd;
			}
			else //If the player is on top of the GameObject
			{
				//printf("ON TOP %f, %f\n", coll.center.y - (coll.dimensions.y / 2), goCollider->dimensions.y);
				transform.SetPosition(transform.GetPosition().x, (float)(goCollider->dimensions.y + (coll.dimensions.y / 2)), transform.GetPosition().z); //Move the player to the top of the GameObject
				transform.MoveRelativeAxes(fwdSpeed, sideSpeed, 0);
				isOnGameObject = true;
				goto breakEnd;
			}
		}
	}
	
	//Move relative to direction we're facing, with no movement on the Y axis
	//transform.MoveRelativeAxes(fwdSpeed, sideSpeed, verticalSpeed * deltaTime);

	//transform.ApplyForce(XMFLOAT3(fwdSpeed*0.1f, sideSpeed*0.1f, verticalSpeed*deltaTime*0.1f));

	breakEnd: //This is super useful and I'm sad I didn't know about it sooner
	{
		//These brackets totally aren't a janky workaround at all
	}
}

void Player::UpdateMouseInput(float xAxis, float yAxis)
{
	//Rotate and clamp vertically between -PI/2 and PI/2 (it can bug out at the extremes so added padding)
	transform.RotateClamped(yAxis / mouseSensitivity, xAxis / mouseSensitivity, 0, -XM_PIDIV2+0.1f, XM_PIDIV2-0.1f);
}

void Player::Jump()
{
	//Make sure not already in air and jump button was not previously held
	//Give a small cushion from ground where players can jump again, this feels way better
	//If the player is on another GameObject, just jump
	if ((!jumpButtonHeld && transform.GetPosition().y <= playerHeight + 0.1f) || (!jumpButtonHeld && isOnGameObject)) {
		verticalSpeed = 2;
	}
}

//Update projection matrix on window resize to match aspect ratio
void Player::UpdateProjectionMatrix(unsigned int width, unsigned int height) {
	//Store projection matrix with new aspect ratio
	XMStoreFloat4x4(&projectionMatrix,
		XMMatrixTranspose(//Transpose for HLSL
			XMMatrixPerspectiveFovLH(
				0.25f * XM_PI,//90 degrees fov
				(float)width/height,//Aspect ratio
				0.1f,//Near clip plane distance
				100.0f//Far clip plane distance
	)));
}

//Update view matrix with current position and direction
void Player::UpdateViewMatrix()
{
	//Only update when transform properties have been modified
	if (!transform.MatrixNeedsUpdate()) return;

	//Store view matrix based on transform's position and direction
	XMStoreFloat4x4(&viewMatrix,
		XMMatrixTranspose(//Transpose for HLSL
			XMMatrixLookToLH(
				XMLoadFloat3(&transform.GetPosition()),//Position
				XMLoadFloat3(&transform.GetForward()),//Direction
				XMVectorSet(0, 1, 0, 0)//Up
	)));

	transform.DoneUpdating();//Notify transform that matrix has been updated
}

XMFLOAT4X4 Player::GetViewMatrix()
{
	return viewMatrix;
}

XMFLOAT4X4 Player::GetProjectionMatrix()
{
	return projectionMatrix;
}

//Get the player's health
byte Player::GetHealth()
{
	return health;
}

//Decrement the player's health when they are hit
void Player::DecrementHealth()
{
	health--;
}

bool Player::CheckProjectileCollisions(GameObject other)
{

	//Collision::CheckCollision(other,);
	return false;
}
