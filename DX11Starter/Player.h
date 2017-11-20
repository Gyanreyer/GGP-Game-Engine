#pragma once
#include "GameObject.h"
#include "Collision.h"
#include "ProjectileManager.h"

//ColliderType may be revamped, since it's going to be from the mesh into the gameobject
class ProjectileManager;
class Player: public GameObject
{
public:
	Player();
	Player(Transform trans, unsigned int projectionWidth, unsigned int projectionHeight, ProjectileManager * pm);
	~Player();

	void Update(float deltaTime);
	void UpdateMouseInput(float xAxis, float yAxis);
	
	XMFLOAT3 GetVelocity();
	void SetVelocity(XMFLOAT3 vel);
	void Accelerate(float fwdMagnitude, float sideMagnitude, float vertMagnitude);
	void Accelerate(XMFLOAT3 accelVec);
	void UpdatePhysics(float deltaTime);

	void Jump(); //Jump w/ some sort of physics

	void UpdateProjectionMatrix(unsigned int width, unsigned int height);
	void UpdateViewMatrix();

	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjectionMatrix();

	byte GetHealth();
	void DecrementHealth();

	//Check collider against all player projectile colliders
	bool CheckProjectileCollisions(GameObject other);

	void Shoot();

	void StopFalling(float newY);

private:
	//BYTE VALUES MUST BE BETWEEN 0 AND 255
	//UNSIGNED SHORT VALUES MUST BE BETWEEN 0 AND 65535
	byte movementSpeed; //The camera's movement speed
	unsigned short mouseSensitivity; //Mouse sensitivity, determines the camera's rotation speed

	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

	byte health;

	//Used in next-frame collisions
	float fwdForce; //+ forward, - backward
	float sideForce; //+ right, - left

	//float verticalSpeed;//Current vertical speed of player - 0 when on ground
	float playerHeight;//Height of player's camera view from the ground

	bool jumpButtonHeld;//Whether jump button is being held

	bool isOnGameObject; //Is the player on a GameObject

	bool onGround;

	//Move based on keyboard input
	void UpdateKeyInput(float deltaTime);

	ProjectileManager * projManager;

	XMFLOAT3 velocity;
	float maxVel;
};

