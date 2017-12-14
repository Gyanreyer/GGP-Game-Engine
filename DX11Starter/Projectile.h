#pragma once
#include "GameObject.h"
#include "Emitter.h"

class Projectile :
	public GameObject
{
public:
	Projectile(Mesh * mesh, Material * material, ID3D11Device* device,
		XMFLOAT3 startPos, XMFLOAT3 direction, float speed);
	~Projectile();	

	void Update(float deltaTime);

	float GetTimeAlive();
	Emitter* GetEmitter();

private:
	float moveSpeed;
	XMFLOAT3 startPosition;
	Emitter* trail;
	float timeAlive;
};

