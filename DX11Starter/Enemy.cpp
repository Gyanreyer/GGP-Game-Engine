//Enemies, inherits from GameObject
#include "Enemy.h"

Enemy::Enemy()
{
}

Enemy::Enemy(XMFLOAT3 position, Mesh * mesh, Material * material, ColliderType colliderType, bool isColliderOffset, ID3D11DeviceContext * ctx, byte pointValue, bool moveX, bool moveY) : GameObject(mesh, material, colliderType, isColliderOffset, ctx)
{
	points = pointValue;
	moveXAxis = moveX;
	moveYAxis = moveY;
	transform.SetPosition(position);
	originPos = transform.GetPosition();
}

Enemy::~Enemy()
{
}

void Enemy::Update(float deltaTime)
{
	//I hate this but for now I just need this to work I will rewrite when we clean the engine up
	if (moveXAxis) 
	{
		if (moveRight) 
		{
			transform.Move(1*deltaTime, 0, 0);
			if (transform.GetPosition().x >= originPos.x + xOffset)
				moveRight = false;
		}
		else {
			transform.Move(-1*deltaTime, 0, 0);
			if (transform.GetPosition().x <= originPos.x - xOffset)
				moveRight = true;
		}
	}
	if (moveYAxis)
	{
		if (moveUp)
		{
			transform.Move(0, 1 * deltaTime, 0);
			if (transform.GetPosition().y >= originPos.y + yOffset)
				moveUp = false;
		}
		else {
			transform.Move(0, -1 * deltaTime, 0);
			if (transform.GetPosition().y <= originPos.y - yOffset)
				moveUp = true;
		}
	}
}


//Return how many points this enemy is worth
int Enemy::GetPoints()
{
	return points;
}

