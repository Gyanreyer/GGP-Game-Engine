//Enemies, inherits from GameObject
#include "Enemy.h"

Enemy::Enemy()
{
}

Enemy::Enemy(Transform trans, Mesh * mesh, Material * material, EnemyType eType, byte pointValue, ProjectileManager * pm)
	: GameObject(trans, mesh, material, "Enemy")
{
	transform = trans;

	halfHeight = transform.GetScale().y / 2;

	points = pointValue;
	type = eType;
	originPos = transform.GetPosition();

	movePositive = true;
	offset = XMFLOAT3(1,1,0);
	
	projManager = pm;

	time(&nowTime); //gets current time when game is launched
	lastShotTime = *localtime(&nowTime); //assigns that time to lastShotTime to keep track of the time when shot was last fired
}

Enemy::~Enemy()
{
}

void Enemy::Update(float deltaTime)
{
	//I hate this but for now I just need this to work I will rewrite when we clean the engine up
	if (type == EnemyType::moveX) 
	{
		int sign = (movePositive ? 1 : -1);
		transform.Move(0.5f*sign*deltaTime, 0, 0);

		movePositive = transform.GetPosition().x <= (originPos.x + offset.x*sign);
	}
	else if (type == EnemyType::moveY)
	{
		int sign = (movePositive ? 1 : -1);
		transform.Move(0, 0.5f*sign*deltaTime, 0); 

		movePositive = transform.GetPosition().y <= (originPos.y + offset.y*sign);
	}

	//Old shoot code, moved to GameManager
	//time(&nowTime); //get current time in game

	//double seconds = difftime(nowTime, mktime(&lastShotTime));

	//if (seconds >= 4)
	//{
	//	Shoot();
	//}
}

void Enemy::Shoot()
{
	ShootDirection(transform.GetForward());
}

void Enemy::ShootDirection(XMFLOAT3 dir)
{
	XMFLOAT3 shootPos = transform.GetPosition();

	if (collider.isOffset)
		shootPos.y += halfHeight;

	//shootPos.z += (collider.dimensions.z / 2); //Adjust the Z position so enemies don't merk themselves

	projManager->SpawnEnemyProjectile(shootPos, dir);

	//This is theoretically pointless if we're using a timer to determine when shots should be fired - we already got the time
	//time(&nowTime); //gets current time when shot is launched
	lastShotTime = *localtime(&nowTime); //assigns that time to lastShotTime to keep track of the time when shot was last fired
}


//Return how many points this enemy is worth
int Enemy::GetPoints()
{
	return points;
}

//Get the time
time_t * Enemy::GetNowTime()
{
	return &nowTime;
}

//Get the last time the enemy shot
tm * Enemy::GetLastShotTime()
{
	return &lastShotTime;
}

