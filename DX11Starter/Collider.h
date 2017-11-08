//Colliders for entities
#pragma once
#include "Transform.h"
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
	//Using optional params to keep things cleaner
	Collider(ColliderType coltype, XMFLOAT3 cent, XMFLOAT3 dims, bool isColliderOffset = false, bool trigger = false)
	{
		collType = coltype;
		isTrigger = trigger;
		isOffset = isColliderOffset;

		//Take scale from transform and divide by two for dimensions
		XMStoreFloat3(&dimensions, XMLoadFloat3(&dims) / 2);//DIVIDE HERE BECAUSE NOT DOING IT HERE HAS ALREADY TORN THIS FAMILY APART
		center = cent;//Get position from transform

		if (isOffset) 
		{
			//Right now, this assumes that the collider is at the "feet" of a model
			//If the need arises, this can be generalized
			//This should only be true on some enemies and environmental GameObjects
			center.y += dimensions.y;
		}
	}
};