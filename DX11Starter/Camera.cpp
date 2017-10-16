#include "Camera.h"

Camera::Camera()
{
}

Camera::Camera(unsigned int width, unsigned int height)
{
	transform = Transform(XMFLOAT3(0,0,-5),XMFLOAT3(0,0,0),XMFLOAT3());

	UpdateProjectionMatrix(width, height);

	rotationSpeed = .5f; //The camera's rotation speed
}

Camera::~Camera()
{
}

Transform * Camera::GetTransform()
{
	return &transform;
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	UpdateViewMatrix();//Make sure viewMatrix is up to date before returning
	return viewMatrix;
}

XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

//Updates the camera's view matrix
void Camera::Update(float deltaTime)
{
	UpdateViewMatrix();

	KeyboardInput(deltaTime); //Handle keyboard input
}

void Camera::UpdateProjectionMatrix(unsigned int width, unsigned int height)
{
	//Create and store projection matrix based on given width/height
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(
		XMMatrixPerspectiveFovLH(0.25f * XM_PI,		// Field of View Angle
			(float)width / height,		// Aspect ratio
			0.1f,	// Near clip plane distance
			100.0f))	// Far clip plane distance
	);
}

//Handles mouse input
void Camera::MouseInput(float xAxis, float yAxis)
{
	//Scale the rotations in terms of radians
	//FPS controlls, mouse moves in the direction the camera moves
	float xRadianized = xAxis / 200;
	float yRadianized = yAxis / 200;

	XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(yRadianized * rotationSpeed, xRadianized * rotationSpeed, 0);

	XMVECTOR forward = XMVectorSet(0, 0, 1, 0);
	XMVECTOR right = XMVectorSet(1, 0, 0, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);

	XMVECTOR updatedForward = XMVector3Rotate(forward, quaternion);
	XMVECTOR updatedRight = XMVector3Rotate(right, quaternion);
	XMVECTOR updatedUp = XMVector3Rotate(up, quaternion);

	XMFLOAT3 storedQuaternion;
	XMStoreFloat3(&storedQuaternion, quaternion);
	//transform.Rotate(storedQuaternion.x, storedQuaternion.y, storedQuaternion.z);

	XMStoreFloat3(&transform.forward, forward);
	XMStoreFloat3(&transform.right, right);
	XMStoreFloat3(&transform.up, up);

	//Rotate around the X and Y axis
	//transform.Rotate(yRadianized * rotationSpeed, xRadianized * rotationSpeed, 0);

	////"Clamp" the rotation within 360 and -360
	////Except these are radians, so clamp between 3.14159 and -3.14159
	////Rotation only occurs on the X and Y axes, so the Z axis can always be 0
	//if (transform.rotation.x > TWO_PI || transform.rotation.x < -TWO_PI)
	//	transform.SetRotation(0, transform.rotation.y, 0);
	//if (transform.rotation.y > TWO_PI || transform.rotation.y < -TWO_PI)
	//	transform.SetRotation(transform.rotation.x, 0, 0);

	////If the camera is twisted around itself
	//if (abs(transform.rotation.x) > PI && abs(transform.rotation.y) > PI)
	//{
	//	if (transform.rotation.x > transform.rotation.y)
	//		transform.SetRotation(abs(transform.rotation.x) - PI, 0, 0);
	//	else if (transform.rotation.x < transform.rotation.y)
	//		transform.SetRotation(0, abs(transform.rotation.y) - PI, 0);
	//	else
	//		transform.SetRotation(0, 0, 0);
	//}

	printf("Rotation ");
	printVector(transform.rotation);
	//printf("Up ");
	//printVector(transform.up);
	//printf("Forward ");
	//printVector(transform.forward);
	//printf("Right ");
	//printVector(transform.right);
	//printf("\n");
}

//Handles keyboard input
void Camera::KeyboardInput(float deltaTime)
{
	XMVECTOR fwd = XMLoadFloat3(&transform.GetForward());

	XMVECTOR normalizedForward = XMVector3Normalize(fwd); //Create a normalized forward vector for movement
	XMVECTOR up = XMVectorSet(0, 1, 0, 0); //The camera's up vector

	float speed = 2.0f * deltaTime; //Movement speed

	//How much to move by in each direction this frame
	float forwardSpeed = 0.0f;
	float sideSpeed = 0.0f;
	float upSpeed = 0.0f;

	//Keyboard inputs use a bit mask because we only need the high bit
	if (GetAsyncKeyState('W') & 0x8000) {
		forwardSpeed += speed; //If player pressed W to go forward, add to forward speed
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		forwardSpeed -= speed; //If player pressed S to go backwards, subtract from forward speed
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		sideSpeed += speed; //If player pressed A to go left, add to side speed
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		sideSpeed -= speed; //If player pressed D to go right, subtract from side speed
	}
	if (GetAsyncKeyState('Z') & 0x8000) {
		upSpeed += speed; //If player pressed left shift to go up, add to up speed
	}
	if (GetAsyncKeyState('X') & 0x8000) {
		upSpeed -= speed; //If player pressed left control to go down, subtract from up speed
	}

	transform.MoveRelative(forwardSpeed, sideSpeed, upSpeed); //Move the camera with directional speeds from input
	/*
	//Move the camera for WASD controls
	//Space and X control vertical movement
	//Move forward
	if (GetAsyncKeyState('W') & 0x8000)
		transform.SetPosition(normalizedForward * movementSpeed * deltaTime);
	//Strafe left
	if (GetAsyncKeyState('A') & 0x8000)
		transform.SetPosition(XMVector3Cross(normalizedForward, up) * movementSpeed * deltaTime); //Cross the forward vector and the up axis
	//Move backward
	if (GetAsyncKeyState('S') & 0x8000)
		transform.SetPosition(-normalizedForward * movementSpeed * deltaTime);
	//Strafe right
	if (GetAsyncKeyState('D') & 0x8000)
		transform.SetPosition(-XMVector3Cross(normalizedForward, up) * movementSpeed * deltaTime); //Cross the forward vector and the up axis
	//Move down
	if (GetAsyncKeyState('X') & 0x8000)
		transform.SetPosition(-up * movementSpeed * deltaTime);
	//Move up
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		transform.SetPosition(up * movementSpeed * deltaTime);*/
}

//Update view matrix if transform has changed
void Camera::UpdateViewMatrix()
{
	if (transform.MatrixNeedsUpdate()) {
		//Calculate view matrix with XMMatrixLookToLH
		XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(
			XMMatrixLookToLH(XMLoadFloat3(&transform.GetPosition()),
				XMLoadFloat3(&transform.GetForward()),
				XMVectorSet(0,1,0,0))));

		transform.DoneUpdating();//Notify transform that matrix successfully updated
	}
}
