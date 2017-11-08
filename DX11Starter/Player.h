#pragma once
#include <Windows.h>
#include "GameObject.h"
#include "Collision.h"

//ColliderType may be revamped, since it's going to be from the mesh into the gameobject
class Player: public GameObject
{
public:
	Player();
	Player(Transform trans, unsigned int projectionWidth, unsigned int projectionHeight);
	~Player();

	void Update(float deltaTime);
	void UpdateMouseInput(float xAxis, float yAxis);

	void Jump(); //Jump w/ some sort of physics

	void UpdateProjectionMatrix(unsigned int width, unsigned int height);
	void UpdateViewMatrix();

	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjectionMatrix();

	byte GetHealth();
	void DecrementHealth();

	//Check collider against all player projectile colliders
	bool CheckProjectileCollisions(GameObject other);

private:
	//BYTE VALUES MUST BE BETWEEN 0 AND 255
	//UNSIGNED SHORT VALUES MUST BE BETWEEN 0 AND 65535
	byte movementSpeed; //The camera's movement speed
	unsigned short mouseSensitivity; //Mouse sensitivity, determines the camera's rotation speed

	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

	byte health;

	float verticalSpeed;//Current vertical speed of player - 0 when on ground
	float playerHeight;//Height of player's camera view from the ground

	bool jumpButtonHeld;//Whether jump button is being held

	bool isOnGameObject; //Is the player on a GameObject

	bool onGround;

	//Move based on keyboard input
	void UpdateKeyInput(float deltaTime);

	void StopFalling();
	bool CheckCollisions(float deltaTime);//This should probably be handled elsewhere
};

