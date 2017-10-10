#pragma once
#include <DirectXMath.h>
#include "Transform.h"

using namespace DirectX;

class Camera
{
public:
	Camera();
	Camera(unsigned int width, unsigned int height);
	~Camera();

	void UpdateProjectionMatrix(unsigned int width, unsigned int height);//Update projection matrix using given width and height
	void UpdateViewMatrix();

	Transform * GetTransform();//Get reference to transform

	//Getters for matrices
	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjectionMatrix();

private:
	Transform transform;

	XMFLOAT4X4 projectionMatrix;//Matrix to project cam's view in 2d for rendering
	XMFLOAT4X4 viewMatrix;
};