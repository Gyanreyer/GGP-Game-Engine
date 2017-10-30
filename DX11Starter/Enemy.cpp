//Enemies, inherits from GameObject
#include "Enemy.h"

Enemy::Enemy()
{
}

Enemy::Enemy(Mesh * mesh, Material * material, ColliderType colliderType, ID3D11DeviceContext * ctx) : GameObject(mesh, material, colliderType, ctx)
{
}

Enemy::~Enemy()
{
}