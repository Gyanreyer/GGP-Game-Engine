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
//This is not your average, everyday C. This is... ADVANCED C
//(Meaning C++ structs can have functions inside of them)
struct Collider
{
	ColliderType collType;
	XMFLOAT3 center; //The position of the collider
	XMFLOAT3 dimensions; //Collider dimensions
	bool isTrigger; //Is this collider a trigger
	bool isOffset; //Is this collider offset

	//Constructor
	Collider()
	{
		collType = ColliderType::NONE;
		center = XMFLOAT3(0, 0, 0);
		dimensions = XMFLOAT3(1, 1, 1);
		isTrigger = false;
		isOffset = false;
	}

	//Parameterized constructor 
	///Make Overload for if collider has offset
	Collider(ColliderType coltype, XMFLOAT3 cent, XMFLOAT3 dims, bool trigger, bool isColliderOffset)
	{
		collType = coltype;

		//If the model's central point is offset
		//This should only be true on some enemies and environmental GameObjects
		if (!isColliderOffset)
			center = cent;
		else
			//Right now, this assumes that the collider is at the "feet" of a model
			//If the need arises, this can be generalized
			center = XMFLOAT3(cent.x, cent.y + (dims.y / 2), cent.z);

		dimensions = XMFLOAT3(dims.x / 2, dims.y / 2, dims.z / 2); //DIVIDE HERE BECAUSE NOT DOING IT HERE HAS ALREADY TORN THIS FAMILY APART
		isTrigger = trigger;
		isOffset = isColliderOffset;
	}
};