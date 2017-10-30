//Enemies, inherits from GameObject
#pragma once
#include "GameObject.h"

class Enemy : public GameObject
{
public:
	Enemy();
	Enemy(Mesh * mesh, Material * material, ColliderType colliderType, ID3D11DeviceContext * ctx);
	~Enemy();
};