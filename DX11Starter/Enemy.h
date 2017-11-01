//Enemies, inherits from GameObject
#pragma once
#include "GameObject.h"

class Enemy : public GameObject
{
public:
	Enemy();
	Enemy(Mesh * mesh, Material * material, ColliderType colliderType, bool isColliderOffset, ID3D11DeviceContext * ctx, byte pointValue);
	~Enemy();

	//Return how many points this enemy is worth
	int GetPoints();

private:
	//The number of points this enemy is worth
	int points;
};