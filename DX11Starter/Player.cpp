#include "Player.h"
#include "GameManager.h"

Player::Player()
{
}

Player::Player(Transform trans, unsigned int projectionWidth, unsigned int projectionHeight)
	: GameObject(trans, BOX)
{
	//BYTE VALUES MUST BE BETWEEN 0 AND 255
	//UNSIGNED SHORT VALUES MUST BE BETWEEN 0 AND 65535
	movementSpeed = 2; //The camera's movement speed
	mouseSensitivity = 400; //Mouse sensitivity, determines the camera's rotation speed

	health = 3;

	UpdateProjectionMatrix(projectionWidth,projectionHeight);

	playerHeight = trans.GetScale().y;

	transform.SetMaxVelocity(3.0f);

	onGround = true;
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

	if (yPos > playerHeight && !isOnGameObject)
	{
		onGround = false;
		transform.ApplyForceRelative(0,0,-5*deltaTime);
	}
	else if (yPos < playerHeight)
	{
		onGround = true;
		//isOnGameObject = false;

		XMFLOAT3 newPos = transform.GetPosition();
		newPos.y = playerHeight;
		transform.SetPosition(newPos);

		StopFalling();
	}

	//Update collider information
	//Collider probably won't be offset, so we won't even worry about that
	collider.center = transform.GetPosition();
	collider.dimensions = transform.GetScale();

	transform.UpdatePhysics(deltaTime);
}

void Player::UpdateKeyInput(float deltaTime)
{
	fwdForce = 0;//+ forward, - backward
	sideForce = 0;//+ right, - left

	//Move forward on XZ plane when W pressed
	if (GetAsyncKeyState('W') & 0x8000) {
		fwdForce += movementSpeed;// *deltaTime;
	}
	//Move backward on XZ plane when S pressed
	if (GetAsyncKeyState('S') & 0x8000) {
		fwdForce -= movementSpeed;// *deltaTime;
	}
	//Move right relative to forward on XZ plane when D pressed
	if (GetAsyncKeyState('D') & 0x8000) {
		sideForce -= movementSpeed;// *deltaTime;
	}
	//Move left relative to forward on XZ plane when A pressed
	if (GetAsyncKeyState('A') & 0x8000) {
		sideForce += movementSpeed;// *deltaTime;
	}
	//Jump when spacebar pressed
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		Jump();
		jumpButtonHeld = true;
	}
	else {
		jumpButtonHeld = false;
	}
	
	transform.ApplyForceRelative(fwdForce, sideForce, 0);

	if (CheckCollisions(deltaTime))
	{
		transform.SetVelocity(XMFLOAT3(0, transform.GetVelocity().y, 0));
	}
	//else
	//{
	//	transform.ApplyForceRelative(fwdForce, sideForce, 0);
	//}
}

void Player::StopFalling()
{
	XMFLOAT3 vel = transform.GetVelocity();
	vel.y = 0;
	transform.SetVelocity(vel);//Stop downward velocity
}

bool Player::CheckCollisions(float deltaTime)
{
	isOnGameObject = false;

	//GAMEOBJECT COLLISIONS
	vector<GameObject>* goVector = GameManager::getInstance().GetGameObjectVector(); //Get the instance of the GameManager
	printf("%d\n", onGround);
	//Start at 1, 0 is the ground
	for (byte i = 1; i < goVector->size(); i++)
	{
		//WITH PLAYER
		Collider* goCollider = (*goVector)[i].GetCollider(); //The GameObject's collider

		if ((goCollider->collType == BOX && Collision::CheckCollisionBoxBox(goCollider, &collider)) || (goCollider->collType == SPHERE && Collision::CheckCollisionSphereBox(goCollider, &collider)))
		{
			if (onGround)//&& transform.GetVelocity().y >= 0) //If the player isn't on top of the GameObject
			{
				//There shouldn't be any breakage due to that small offset value, but I'm leaving debug stuff here just in case
				printf("NOT ON TOP %f, %f\n", collider.center.y - (collider.dimensions.y / 2), goCollider->dimensions.y + .005f);
				//transform.SetVelocity(XMFLOAT3(0, transform.GetVelocity().y, 0));
				return true;
			}
			else if (collider.center.y - (collider.dimensions.y / 2) <= goCollider->dimensions.y && !onGround) //If the player is on top of the GameObject
			{
				printf("ON TOP %f, %f\n", collider.center.y - (collider.dimensions.y / 2), goCollider->dimensions.y + .005f);
				transform.SetPosition(transform.GetPosition().x, goCollider->dimensions.y + (collider.dimensions.y / 2), transform.GetPosition().z); //Move the player to the top of the GameObject
				isOnGameObject = true;
				StopFalling();

				return true;
			}
		}
	}

	return false; //Not colliding with anything
}

void Player::UpdateMouseInput(float xAxis, float yAxis)
{
	//Rotate and clamp vertically between -PI/2 and PI/2 (it can bug out at the extremes so added padding)
	transform.RotateClamped(yAxis / mouseSensitivity, xAxis / mouseSensitivity, 0, -XM_PIDIV2+0.1f, XM_PIDIV2-0.1f);
}

void Player::Jump()
{
	if (jumpButtonHeld) return;

	//Make sure not already in air and jump button was not previously held
	//Give a small cushion from ground where players can jump again, this feels way better
	//If the player is on another GameObject, just jump
	if (onGround || transform.GetPosition().y <= playerHeight + 0.1f)
	{
		transform.ApplyForce(0,3,0);
		onGround = false;
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
