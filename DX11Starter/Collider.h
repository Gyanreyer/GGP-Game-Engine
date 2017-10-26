//Colliders for entities
#pragma once

#include <DirectXMath.h> //DirctX math functions

using namespace DirectX;

//The type of collider this is
enum ColliderType {
	BOX =0,
	SPHERE =1,
	MESH =2, //Probably will never implement
	NONE = 3
};

//Base collider
struct Collider
{
	ColliderType collType;
	XMFLOAT3 center; //The position of the collider
	bool isTrigger; //Is this collider a trigger

	//Constructor
	Collider()
	{
		collType = ColliderType::NONE;
		center = XMFLOAT3();
		isTrigger = false;
	}

	//Constructor 
	Collider(XMFLOAT3 cent, ColliderType coltype, bool trigger)
	{
		center = cent;
		isTrigger = trigger;
		collType = coltype;
	}
};

//Box collider
struct BoxCollider : Collider
{
	XMFLOAT3 dimensions; //Collider dimensions

	//Constructor 
	BoxCollider(XMFLOAT3 cent, bool trigger, XMFLOAT3 dim) : Collider(cent, ColliderType::BOX, trigger)
	{
		dimensions = dim;
	}
};

//Sphere collider
struct SphereCollider : Collider
{
	float radius; //Collider radius

	//Constructor 
	SphereCollider(XMFLOAT3 cent, bool trigger, float rad) : Collider(cent,  ColliderType::SPHERE, trigger)
	{
		radius = rad;
	}
};