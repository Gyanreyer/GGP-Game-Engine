//Colliders for entities
#pragma once

#include <DirectXMath.h> //DirctX math functions

using namespace DirectX;

//The type of collider this is
enum ColliderType {
	BOX,
	SPHERE,
	MESH, //Probably will never implement
	NONE
};

//Base collider
struct Collider
{
	XMFLOAT3 center; //The position of the collider
	bool isTrigger; //Is this collider a trigger

	//Constructor 
	Collider(XMFLOAT3 cent, bool trigger)
	{
		center = cent;
		isTrigger = trigger;
	}
};

//Box collider
struct BoxCollider : Collider
{
	XMFLOAT3 dimensions; //Collider dimensions

	//Constructor 
	BoxCollider(XMFLOAT3 cent, bool trigger, XMFLOAT3 dim) : Collider(cent, trigger)
	{
		dimensions = dim;
	}
};

//Sphere collider
struct SphereCollider : Collider
{
	float radius; //Collider radius

	//Constructor 
	SphereCollider(XMFLOAT3 cent, bool trigger, float rad) : Collider(cent, trigger)
	{
		radius = rad;
	}
};