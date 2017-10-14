#pragma once
#include <DirectXMath.h>
#include <Windows.h> //Input
#include "Transform.h"

using namespace DirectX;

class Camera
{
public:
	Camera();
	Camera(unsigned int width, unsigned int height);
	~Camera();

	Transform * GetTransform();//Get reference to transform

	//Getters for matrices
	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjectionMatrix();

	void Update(float deltaTime); //Updates the camera's view matrix
	void UpdateProjectionMatrix(unsigned int width, unsigned int height); //Update projection matrix using given width and height
	void MouseInput(float xAxis, float yAxis); //Handles mouse input

private:
	Transform transform;

	XMFLOAT4X4 projectionMatrix; //Matrix to project cam's view in 2d for rendering
	XMFLOAT4X4 viewMatrix;

	float rotationSpeed; //The camera's rotation speed

	void KeyboardInput(float deltaTime); //Handles keyboard input
	void UpdateViewMatrix();
};