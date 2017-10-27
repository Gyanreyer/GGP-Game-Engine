#include "Player.h"

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

	UpdateProjectionMatrix(projectionWidth,projectionHeight);
}

Player::~Player()
{
}

void Player::Update(float deltaTime)
{
	UpdateKeyInput(deltaTime);
	UpdateViewMatrix();
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
	}

	//Move relative to direction we're facing, with no movement on y axis
	transform.MoveRelativeAxes(fwdSpeed, sideSpeed, 0);
}

void Player::UpdateMouseInput(float xAxis, float yAxis)
{
	//Rotate and clamp vertically between -pi/2 and pi/2
	transform.RotateClamped(yAxis/mouseSensitivity,xAxis/mouseSensitivity,0,-XM_1DIV2PI,XM_1DIV2PI);
}

void Player::Jump()
{
	//Make sure not already in air and then jump somehow
}

void Player::Shoot()
{
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
