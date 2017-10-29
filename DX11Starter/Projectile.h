#pragma once
#include "GameObject.h"
class Projectile :
	public GameObject
{
public:
	Projectile(Mesh * mesh, Material * material, ColliderType colliderType, ID3D11DeviceContext * ctx, 
		XMFLOAT3 startPos, XMFLOAT3 direction, float speed);
	~Projectile();	

	void Update(float deltaTime);

	float GetTimeAlive();

private:
	float moveSpeed;
	XMFLOAT3 startPosition;

	float timeAlive;
};

