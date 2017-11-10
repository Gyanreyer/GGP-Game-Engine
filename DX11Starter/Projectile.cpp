#include "Projectile.h"

Projectile::Projectile(Mesh * mesh, Material * material,
	XMFLOAT3 startPos, XMFLOAT3 direction, float speed) :
	GameObject(Transform(startPos,XMFLOAT3(0,0,0),XMFLOAT3(0.02f,0.02f,0.02f)),mesh, material)
{
	timeAlive = 0;
	moveSpeed = speed;//Store speed that will move by each frame
	
	//Make projectile face same direction as player
	transform.SetForward(direction);
	transform.MoveRelative(0.1f,0,0);
}

Projectile::~Projectile()
{
}

//Move forward until out of range
void Projectile::Update(float deltaTime)
{
	//Move forward endlessly every frame
	transform.MoveRelative(moveSpeed*deltaTime,0,0);

	timeAlive += deltaTime;
}

float Projectile::GetTimeAlive()
{
	return timeAlive;
}
