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

	void Update(float deltaTime);
	void UpdateProjectionMatrix(unsigned int width, unsigned int height);//Update projection matrix using given width and height
	void RotateCamera(float xAmount, float yAmount);


	CameraTransform * GetTransform();

	//Getters for matrices
	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjectionMatrix();

	

private:
	CameraTransform transform;

	XMFLOAT4X4 projectionMatrix;//Matrix to project cam's view in 2d for rendering

	float xRot, yRot;
	float speed;
	float originSpeed;
	float doubleSpeed;
	float rotSpeed; //Camera rotation speed
};