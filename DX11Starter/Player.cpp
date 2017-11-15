#include "Player.h"

Player::Player()
{
}

Player::Player(Transform trans, unsigned int projectionWidth, unsigned int projectionHeight, ProjectileManager * pm)
	: GameObject(trans, BOX, "Player")
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

	projManager = pm;
}

Player::~Player()
{
}

void Player::Update(float deltaTime)
{
	transform.UpdatePhysics(deltaTime);

	UpdateKeyInput(deltaTime);
	UpdateViewMatrix();

	//The player's Y position is at it's feet
	float yPos = transform.GetPosition().y - playerHeight;

	//Changed to not use playerHeight, since the Y position is now at it's feet
	if (yPos > 0 && !isOnGameObject)
	{
		transform.ApplyForceRelative(0, 0, -5*deltaTime);
	}
	else if (yPos < 0)
	{
		onGround = true;

		XMFLOAT3 newPos = transform.GetPosition();
		newPos.y = playerHeight;
		transform.SetPosition(newPos);

		StopFalling();
	}

	isOnGameObject = false;
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
	
	transform.ApplyForceRelative(fwdForce, sideForce, 0);
}

void Player::StopFalling()
{
	XMFLOAT3 vel = transform.GetVelocity();
	vel.y = 0;
	transform.SetVelocity(vel);//Stop downward velocity
}

void Player::Shoot()
{
	projManager->SpawnPlayerProjectile(transform.GetPosition(), transform.GetForward());
}

void Player::SetIsOnGO(bool onGO)
{
	isOnGameObject = onGO;
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
	if (onGround || isOnGameObject || transform.GetPosition().y <= playerHeight + 0.1f)
	{
		transform.ApplyForce(0, 3, 0);
		onGround = false;
		isOnGameObject = false;
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
