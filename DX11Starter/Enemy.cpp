//Enemies, inherits from GameObject
#include "Enemy.h"

Enemy::Enemy()
{
}

Enemy::Enemy(Transform tForm, Mesh * mesh, Material * material, ColliderType colliderType, bool isColliderOffset, ID3D11DeviceContext * ctx, byte pointValue, bool moveX, bool moveY, ProjectileManager* projManager) : GameObject(mesh, material, colliderType, isColliderOffset, ctx)
{
	transform = tForm;
	XMFLOAT3 position = transform.GetPosition();

	points = pointValue;
	moveXAxis = moveX;
	moveYAxis = moveY;
	originPos = position;
	isOffset = isColliderOffset;

	pManager = projManager; //Save the reference to the ProjectileManager
	
	time(&nowTime); //gets current time when game is launched
	lastShotTime = *localtime(&nowTime); //assigns that time to lastShotTime to keep track of the time when shot was last fired

	//Move the shoot point if the collider is offset
	if (!isOffset)
	{
		//Make player shoot
		pManager->SpawnEnemyProjectile(position, transform.GetForward());
	}
	else
	{
		XMFLOAT3 shootPos;
		XMStoreFloat3(&shootPos, XMLoadFloat3(&XMFLOAT3(originPos.x, originPos.y + (transform.GetScale().y / 2), originPos.z)));

		pManager->SpawnEnemyProjectile(shootPos, transform.GetForward());
	}
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

	if (seconds >= 4)
	{
		//Move the shoot point if the collider is offset
		if (!isOffset)
		{
			//Make player shoot
			pManager->SpawnEnemyProjectile(transform.GetPosition(), transform.GetForward());
		}
		else
		{
			XMFLOAT3 shootPos;
			XMStoreFloat3(&shootPos, XMLoadFloat3(&XMFLOAT3(originPos.x, originPos.y + (transform.GetScale().y / 2), originPos.z)));

			pManager->SpawnEnemyProjectile(shootPos, transform.GetForward());
		}

		time(&nowTime); //gets current time when shot is launched
		lastShotTime = *localtime(&nowTime); //assigns that time to lastShotTime to keep track of the time when shot was last fired
	}
}


//Return how many points this enemy is worth
int Enemy::GetPoints()
{
	return points;
}

