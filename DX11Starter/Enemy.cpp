//Enemies, inherits from GameObject
#include "Enemy.h"
#include "GameManager.h"

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
	
	time(&nowTime); //gets current time when game is launched
	lastShotTime = *localtime(&nowTime); //assigns that time to lastShotTime to keep track of the time when shot was last fired

	GameManager::getInstance().GetProjectileManager()->SpawnEnemyProjectile(transform.GetPosition(), transform.GetRotation());
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

	time(&nowTime); //get current time in game

	double seconds = difftime(nowTime, mktime(&lastShotTime));
	if (seconds >= 4) {
		GameManager::getInstance().GetProjectileManager()->SpawnEnemyProjectile(transform.GetPosition(), transform.GetRotation());
		time(&nowTime); //gets current time when shot is launched
		lastShotTime = *localtime(&nowTime); //assigns that time to lastShotTime to keep track of the time when shot was last fired
	}
}


//Return how many points this enemy is worth
int Enemy::GetPoints()
{
	return points;
}

