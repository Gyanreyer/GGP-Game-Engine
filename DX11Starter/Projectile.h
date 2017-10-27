#pragma once
#include "GameObject.h"
class Projectile :
	public GameObject
{
//TODO: Implement projectile manager to manage spawning/updating/removing projectiles
public:
	//Investigate ways to clone a premade projectile, sorta Unity prefab style?
	Projectile(Mesh * mesh, Material * material, ColliderType colliderType, ID3D11DeviceContext * ctx);
	~Projectile();	

	void Fire(XMFLOAT3 startPos, XMFLOAT3 direction, float speed);
	void Update(float deltaTime);

	float GetDistanceTraveled();

private:
	float moveSpeed;
	XMFLOAT3 startPosition;
};

