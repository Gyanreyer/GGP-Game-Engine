#pragma once
#include "GameObject.h"
#include <Windows.h>

class Player: public GameObject
{
public:
	Player();
	Player(ColliderType colliderType,unsigned int projectionWidth, unsigned int projectionHeight);
	~Player();

	void Update(float deltaTime);

	//Move based on keyboard input
	void UpdateKeyInput(float deltaTime);
	void UpdateMouseInput(float xAxis, float yAxis);

	void Jump();//Jump w/ some sort of physics
	void Shoot();//Spawn a projectile that moves in direction player is facing

	void UpdateProjectionMatrix(unsigned int width, unsigned int height);
	void UpdateViewMatrix();

	XMFLOAT4X4 * GetViewMatrix();
	XMFLOAT4X4 * GetProjectionMatrix();

private:
	//BYTE VALUES MUST BE BETWEEN 0 AND 255
	//UNSIGNED SHORT VALUES MUST BE BETWEEN 0 AND 65535
	byte movementSpeed; //The camera's movement speed
	unsigned short mouseSensitivity; //Mouse sensitivity, determines the camera's rotation speed

	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
};

