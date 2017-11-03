#include "Projectile.h"

Projectile::Projectile(Mesh * mesh, Material * material, ColliderType colliderType, ID3D11DeviceContext * ctx,
	XMFLOAT3 startPos, XMFLOAT3 direction, float speed) :
	GameObject(mesh, material, colliderType, false, ctx)
{
	timeAlive = 0;
	moveSpeed = speed;//Store speed that will move by each frame
	
	//Move projectile to start pos facing same direction as player
	startPosition = startPos;
	transform.SetPosition(startPos);
	transform.SetForward(direction);
	transform.SetScale(0.02f);

	transform.MoveRelative(0.1f,0,0);

	//Reassign collider information here, it gets set to default values when going
	//through the GameObject constructor, but doesn't update when the transforms are changed
	//above until the GameObject's UpdateWorldMatrix function is called. That doesn't happen
	//before collisions can occur, so we just set it here to avoid that entirely.
	//(This resolves the issue of the projectile spawning at the center and immediately
	//getting destroyed because it collides with the ground)
	coll.center = startPos;
	coll.dimensions = XMFLOAT3(0.1f, 0.1f, 0.1f);
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
