//Enemies, inherits from GameObject
#pragma once
#include "GameObject.h"
#include "ProjectileManager.h"
#include <time.h>

enum EnemyType {
	noMove,
	moveX,
	moveY
};

/*Change Notes
** Constructor should be for transform, Mesh , Material, pointValue, EnemyType
*move any collider references (collidertype/offset) into the mesh class
*context would be in renderer class
*/
class Enemy : public GameObject
{
public:
	Enemy();
	Enemy(Transform trans, Mesh * mesh, Material * material, ProjectileManager* projManager, byte pointValue, EnemyType eType);
	~Enemy();
	void Update(float deltaTime);

	 void Shoot();
	 void ShootDirection(XMFLOAT3 dir);

	//Return how many points this enemy is worth
	int GetPoints();

private:
	//Enemy Shoot Timer
	time_t nowTime;
	tm lastShotTime;

	//ProjectileManager reference
	ProjectileManager* pManager;

	//The number of points this enemy is worth
	int points;

	EnemyType type;

	XMFLOAT3 originPos;

	XMFLOAT3 offset;

	bool movePositive;

	//If the collider is offset
	//bool isOffset;

	float halfHeight;
/*
	int xOffset = 2;
	int yOffset = 2;
	bool moveXAxis = false;
	bool moveRight = true;
	bool moveUp = true;
	bool moveYAxis = false;
	*/
};