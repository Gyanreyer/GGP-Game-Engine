#include "Projectile.h"

Projectile::Projectile(Mesh * mesh, Material * material, ColliderType colliderType, ID3D11DeviceContext * ctx,
	XMFLOAT3 startPos, XMFLOAT3 rotation, float speed) :
	GameObject(mesh, material, colliderType, false, ctx)
{
	timeAlive = 0;
	moveSpeed = speed;//Store speed that will move by each frame
	
	//Move projectile to start pos facing same direction as player
	startPosition = startPos;
	transform.SetPosition(startPos);
	transform.SetRotation(rotation);
	transform.SetScale(0.1f);
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
