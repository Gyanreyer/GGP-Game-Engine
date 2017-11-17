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
	Enemy(Transform trans, Mesh * mesh, Material * material, EnemyType eType, byte pointValue, ProjectileManager * pm);
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

	//The number of points this enemy is worth
	int points;

	EnemyType type;

	XMFLOAT3 originPos;

	XMFLOAT3 offset;

	bool movePositive;

	float halfHeight;

	ProjectileManager * projManager;
};