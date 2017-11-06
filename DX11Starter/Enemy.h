//Enemies, inherits from GameObject
#pragma once
#include "GameObject.h"
#include "ProjectileManager.h"
#include <time.h>

/*Change Notes
** Constructor should be for transform, Mesh , Material, pointValue, EnemyType
*move any collider references (collidertype/offset) into the mesh class
*context would be in renderer class
*/
class Enemy : public GameObject
{
public:
	Enemy();
	Enemy(Transform tForm, Mesh * mesh, Material * material, ProjectileManager* projManager, byte pointValue, bool moveX, bool moveY, ID3D11DeviceContext * ctx);
	~Enemy();
	void Update(float deltaTime);

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

	XMFLOAT3 originPos;
	int xOffset = 2;
	int yOffset = 2;
	bool moveXAxis = false;
	bool moveRight = true;
	bool moveUp = true;
	bool moveYAxis = false;
};