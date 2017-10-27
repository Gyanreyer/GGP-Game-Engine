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
	ColliderType collType;
	XMFLOAT3 center; //The position of the collider
	XMFLOAT3 dimensions; //Collider dimensions
	bool isTrigger; //Is this collider a trigger

	//Constructor
	Collider()
	{
		collType = ColliderType::NONE;
		center = XMFLOAT3(0, 0, 0);
		dimensions = XMFLOAT3(1, 1, 1);
		isTrigger = false;
	}

	//Parameterized constructor 
	Collider(ColliderType coltype, XMFLOAT3 cent, XMFLOAT3 dims, bool trigger)
	{
		collType = coltype;
		center = cent;
		dimensions = XMFLOAT3(dims.x / 2, dims.y / 2, dims.z / 2); //DIVIDE HERE BECAUSE NOT DOING IT HERE HAS ALREADY TORN THIS FAMILY APART
		isTrigger = trigger;
	}
};