//Enemies, inherits from GameObject
#include "Enemy.h"

Enemy::Enemy()
{
}

Enemy::Enemy(Mesh * mesh, Material * material, ColliderType colliderType, bool isColliderOffset, ID3D11DeviceContext * ctx, byte pointValue) : GameObject(mesh, material, colliderType, isColliderOffset, ctx)
{
	points = pointValue;
}

Enemy::~Enemy()
{
}

//Return how many points this enemy is worth
int Enemy::GetPoints()
{
	return points;
}

