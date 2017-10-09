#include "Camera.h"

Camera::Camera()
{
}

Camera::Camera(unsigned int width, unsigned int height)
{
	transform = CameraTransform();

	UpdateProjectionMatrix(width, height);
}

Camera::~Camera()
{
}

CameraTransform * Camera::GetTransform()
{
	return &transform;
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	return transform.GetViewMatrixFloat();
}

XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
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