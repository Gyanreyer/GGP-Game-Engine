//Enemies, inherits from GameObject
#pragma once
#include "GameObject.h"
#include <time.h>

class Enemy : public GameObject
{
public:
	Enemy();
	Enemy(XMFLOAT3 position, Mesh * mesh, Material * material, ColliderType colliderType, bool isColliderOffset, ID3D11DeviceContext * ctx, byte pointValue, bool moveX, bool moveY);
	~Enemy();
	 void Update(float deltaTime);

	//Return how many points this enemy is worth
	int GetPoints();

private:
	//Enemy Shoot Timer
	time_t nowTime;
	tm lastShotTime;

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