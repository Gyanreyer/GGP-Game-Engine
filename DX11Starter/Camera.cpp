///Kevin Idzik
///First person camera
#include "Camera.h"

//Constructor
Camera::Camera()
{
}

//Parameterized constructor
Camera::Camera(unsigned int width, unsigned int height)
{
	XMStoreFloat4x4(&viewMatrix, XMMatrixIdentity()); //Store the identity matrix in the view matrix
	XMStoreFloat4x4(&projectionMatrix, XMMatrixIdentity()); //Store the identity matrix in the projection matrix

	position = XMFLOAT3(0, 0, -5); //Default position is at world origin
	rotation = XMFLOAT3(0, 0, 0); //Default rotation is none
	forward = XMFLOAT3(0, 0, 1); //The camera's forward vector
	right = XMFLOAT3(1, 0, 0); //The camera's right vector
	up = XMFLOAT3(0, 1, 0); //The camera's up vector

	//BYTE VALUES MUST BE BETWEEN 0 AND 255
	//UNSIGNED SHORT VALUES MUST BE BETWEEN 0 AND 65535
	movementSpeed = 2; //The camera's movement speed
	mouseSensitivity = 400; //Mouse sensitivity, determines the camera's rotation speed

	//Initialize the view matrix
	UpdateViewMatrix();

	//Initialize the projection matrix
	UpdateProjectionMatrix(width, height);
}

//Destructor
Camera::~Camera()
{
}

//Get the camera's view matrix
XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

//Get the camera's projection matrix
XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

//Set the camera's rotation
void Camera::SetRotation(XMFLOAT3 rot)
{

}

//Handles keyboard input
void Camera::KeyboardInput(float deltaTime)
{
	//Load vectors from the camera
	XMVECTOR pos = XMLoadFloat3(&position); //Load the position FLOAT3 as an XMVECTOR
	XMVECTOR cameraForward = XMVector3Normalize(XMLoadFloat3(&forward)); //Load the camera's forward vector and normalize it
	XMVECTOR cameraRight = XMVector3Normalize(XMLoadFloat3(&right)); //Load the camera's right vector and normalize it
	XMVECTOR cameraUp = XMVector3Normalize(XMLoadFloat3(&up)); //Load the camera's up vector and normalize it

	//Create a unit up vector for vertical movement and cross product-based forward and backwards movement
	XMVECTOR unitUp = XMVectorSet(0, 1, 0, 0);

	//Keyboard inputs use a bit mask because we only need the high bit
	//Move the camera for WASD controls
	//Space and X control vertical movement
	//FPS controls, this will seem strange without the ground and other objects for context
	//Move forward
	if (GetAsyncKeyState('W') & 0x8000)
		pos += XMVector3Cross(cameraRight, unitUp) * movementSpeed * deltaTime; //Cross the camera's right vector and the unit up vector to move forward
	//Strafe left
	if (GetAsyncKeyState('A') & 0x8000)
		pos += XMVector3Cross(cameraForward, cameraUp) * movementSpeed * deltaTime; //Cross the camera's forward vector and the camera's up vector to move left
	//Move backward
	if (GetAsyncKeyState('S') & 0x8000)
		pos -= XMVector3Cross(cameraRight, unitUp) * movementSpeed * deltaTime; //Cross the camera's right vector and the unit camera's vector to move backwards
	//Strafe right
	if (GetAsyncKeyState('D') & 0x8000)
		pos -= XMVector3Cross(cameraForward, cameraUp) * movementSpeed * deltaTime; //Cross the camera's forward vector and the unit up vector to move right
	//Move down
	if (GetAsyncKeyState('X') & 0x8000)
		pos -= unitUp * movementSpeed * deltaTime; //Use the unit up vector for flying up
	//Move up
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		pos += unitUp * movementSpeed * deltaTime; //Use the unit up vector for flying down

	//Store the position back into an XMVECTOR
	//The camera's forward and right vectors do not need to be stored, they're probably updated in UpdateViewMatrix
	XMStoreFloat3(&position, pos);
}

//Updates the view matrix
void Camera::UpdateViewMatrix()
{
	XMVECTOR updatedRotation = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z); //Create a new rotation quaternion

	//Apply the new rotation matrix to the camera's forward vector
	//Pass in a unit forward vector and the camera's forward vector
	XMLoadFloat3(&forward) = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), updatedRotation);

	//Update the view matrix using the camera's position, forward vector, and a unit up vector
	XMMATRIX updatedViewMatrix = XMMatrixLookToLH(XMLoadFloat3(&position), XMLoadFloat3(&forward), XMVectorSet(0, 1, 0, 0));

	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(updatedViewMatrix)); //Transpose and update the camera's view matrix
}

//Updates the camera's view matrix
void Camera::Update(float deltaTime)
{
	UpdateViewMatrix();

	KeyboardInput(deltaTime); //Handle keybaord input
}

//Updates the projection matrix
void Camera::UpdateProjectionMatrix(unsigned int width, unsigned int height)
{
	//Matches the window's aspect ratio, updates on window resize
	//Field of view angle
	//Aspect ratio
	//Near clip plane distance
	//Far clip plane distance
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * 3.1415926535f, (float)width / height, 0.1f, 100.0f);
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); //Transpose for HLSL
}

//Handles mouse input
void Camera::MouseInput(float xAxis, float yAxis)
{
	//Calculate the rotations, scaled in terms of radians
	//FPS controlls, mouse moves in the direction the camera moves
	rotation.x += yAxis / mouseSensitivity;
	rotation.y += xAxis / mouseSensitivity;

	//Clamp the rotation looking up and down (prevents stupid mouse things)
	if (rotation.x > HALF_PI)
		rotation.x = HALF_PI;
	else if (rotation.x < -HALF_PI)
		rotation.x = -HALF_PI;

	//Calculate quaternion
	XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, 0);

	//Update vectors
	//Pass in unit vectors for the corresponding update along with the quaternion
	XMVECTOR updatedForward = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), quaternion);
	XMVECTOR updatedRight = XMVector3Rotate(XMVectorSet(1, 0, 0, 0), quaternion);
	XMVECTOR updatedUp = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), quaternion);

	//Store the updated vectors
	XMStoreFloat3(&forward, updatedForward);
	XMStoreFloat3(&right, updatedRight);
	XMStoreFloat3(&up, updatedUp);
}