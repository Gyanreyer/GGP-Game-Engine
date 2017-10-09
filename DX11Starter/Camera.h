#pragma once
#include <DirectXMath.h>
#include "CameraTransform.h"

using namespace DirectX;

class Camera
{
public:
	Camera();
	Camera(unsigned int width, unsigned int height);
	~Camera();

	CameraTransform * GetTransform();

	//Getters for matrices
	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjectionMatrix();

	void UpdateProjectionMatrix(unsigned int width, unsigned int height);//Update projection matrix using given width and height

private:
	CameraTransform transform;

	XMFLOAT4X4 projectionMatrix;//Matrix to project cam's view in 2d for rendering

	float xRot, yRot;
};