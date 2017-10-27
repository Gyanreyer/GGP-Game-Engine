#include "Projectile.h"

Projectile::Projectile(Mesh * mesh, Material * material, ColliderType colliderType, ID3D11DeviceContext * ctx):
	GameObject(mesh,material,colliderType,ctx)
{
}

Projectile::~Projectile()
{
}

//Move projectile to start pos facing same direction as player
//Store speed that will move by each frame
void Projectile::Fire(XMFLOAT3 startPos, XMFLOAT3 rotation, float speed)
{
	moveSpeed = speed;
	startPosition = startPos;
	transform.SetPosition(startPos);
	transform.SetRotation(rotation);
}

//Move forward until out of range
void Projectile::Update(float deltaTime)
{
	//Move forward endlessly every frame
	transform.MoveRelative(moveSpeed*deltaTime,0,0);
}

//Get distance traveled from start point since fired
float Projectile::GetDistanceTraveled()
{
	XMFLOAT3 lengthEst;
	//Helper method optimized to get an efficient but not perfectly accurate estimate of dist
	//Its return value is an XMVECTOR of three identical floats for the result, the fuck
	XMStoreFloat3(&lengthEst,
		XMVector3LengthEst(XMLoadFloat3(&transform.GetPosition())-XMLoadFloat3(&startPosition)));

	return lengthEst.x;
}
