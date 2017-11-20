#include "Player.h"

Player::Player()
{
}

Player::Player(Transform trans, unsigned int projectionWidth, unsigned int projectionHeight, ProjectileManager * pm)
	: GameObject(trans, BOX, "Player")
{
	collider.isOffset = true;

	//BYTE VALUES MUST BE BETWEEN 0 AND 255
	//UNSIGNED SHORT VALUES MUST BE BETWEEN 0 AND 65535
	movementSpeed = 2; //The camera's movement speed
	mouseSensitivity = 400; //Mouse sensitivity, determines the camera's rotation speed

	health = 3;

	UpdateProjectionMatrix(projectionWidth,projectionHeight);

	playerHeight = trans.GetScale().y;

	isOnGameObject = true;

	projManager = pm;

	maxVel = 3;

	velocity = XMFLOAT3(0, 0, 0);
}

Player::~Player()
{
}

void Player::Update(float deltaTime)
{
	UpdateKeyInput(deltaTime);

	//Changed to not use playerHeight, since the Y position is now at it's feet
	if (!isOnGameObject)
	{
		Accelerate(0, 0, -5 * deltaTime);
	}
	else
	{
		isOnGameObject = false;
	}
}

void Player::UpdateKeyInput(float deltaTime)
{
	//Global for next-frame collisions
	fwdForce = 0; //+ forward, - backward
	sideForce = 0; //+ right, - left

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
	
	Accelerate(fwdForce, sideForce, 0);
}

void Player::StopFalling(float newY)
{
	if (velocity.y > 0) return;

	velocity.y = 0;

	XMFLOAT3 pos = transform.GetPosition();

	pos.y = newY;

	transform.SetPosition(pos);

	isOnGameObject = true;
}

void Player::Shoot()
{
	XMFLOAT3 shootPos = transform.GetPosition();
	shootPos.y += playerHeight;

	projManager->SpawnPlayerProjectile(shootPos, transform.GetForward());
}

void Player::UpdateMouseInput(float xAxis, float yAxis)
{
	//Rotate and clamp vertically between -PI/2 and PI/2 (it can bug out at the extremes so added padding)
	transform.RotateClamped(yAxis / mouseSensitivity, xAxis / mouseSensitivity, 0, -XM_PIDIV2+0.1f, XM_PIDIV2-0.1f);
}

XMFLOAT3 Player::GetVelocity()
{
	return velocity;
}

void Player::SetVelocity(XMFLOAT3 vel)
{
	velocity = vel;
}

void Player::Accelerate(float fwdMagnitude, float sideMagnitude, float vertMagnitude)
{
	XMFLOAT3 accelVec;

	XMStoreFloat3(&accelVec, XMLoadFloat3(&transform.GetForwardXZ())*fwdMagnitude +
		XMLoadFloat3(&transform.GetRight())*sideMagnitude +
		XMVectorSet(0, 1, 0, 0)*vertMagnitude);

	Accelerate(accelVec);
}

void Player::Accelerate(XMFLOAT3 accelVec)
{
	XMStoreFloat3(&velocity, 
		XMLoadFloat3(&velocity) +
		XMLoadFloat3(&accelVec));

	XMStoreFloat3(&velocity,
		XMVectorSet(0, velocity.y, 0, 0) +
		XMVector3ClampLength(XMVectorSet(velocity.x, 0, velocity.z, 0), 0, maxVel));
}

void Player::UpdatePhysics(float deltaTime)
{
	transform.Move(XMLoadFloat3(&velocity)*deltaTime);

	velocity.x *= deltaTime*.5f;
	velocity.z *= deltaTime*.5f;
}

void Player::Jump()
{
	if (jumpButtonHeld) return;

	//Make sure not already in air and jump button was not previously held
	//Give a small cushion from ground where players can jump again, this feels way better
	//If the player is on another GameObject, just jump
 	if ( isOnGameObject)
	{
		Accelerate(0, 0, 3);
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

	//Modify view pos so that it's at top of player
	XMFLOAT3 viewPos = transform.GetPosition();
	viewPos.y += playerHeight;

	//Store view matrix based on transform's position and direction
	XMStoreFloat4x4(&viewMatrix,
		XMMatrixTranspose(//Transpose for HLSL
			XMMatrixLookToLH(
				XMLoadFloat3(&viewPos),//Position
				XMLoadFloat3(&transform.GetForward()),//Direction
				XMVectorSet(0, 1, 0, 0)//Up
	)));

	transform.DoneUpdating();//Notify transform that matrix has been updated
}

XMFLOAT4X4 Player::GetViewMatrix()
{
	UpdateViewMatrix();
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
	if(health > 0)
		health--;
}

bool Player::CheckProjectileCollisions(GameObject other)
{

	//Collision::CheckCollision(other,);
	return false;
}
