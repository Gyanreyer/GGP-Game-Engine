#include "Camera.h"

Camera::Camera()
{
}

Camera::Camera(unsigned int width, unsigned int height)
{
	transform = Transform(XMFLOAT3(0,0,-5),XMFLOAT3(0,0,0),XMFLOAT3());

	UpdateProjectionMatrix(width, height);
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
