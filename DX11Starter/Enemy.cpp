//Enemies, inherits from GameObject
#include "Enemy.h"

Enemy::Enemy()
{
}

Enemy::Enemy(Mesh * mesh, Material * material, ColliderType colliderType, ID3D11DeviceContext * ctx, byte pointValue) : GameObject(mesh, material, colliderType, ctx)
{
	points = pointValue;
}

Enemy::~Enemy()
{
}

//Return how many points this enemy is worth
byte Enemy::GetPoints()
{
	return points;
}
