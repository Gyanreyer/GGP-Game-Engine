//Resolve collisions between entities
#pragma once
#include "Collider.h"

class Collision
{
public:
	static bool CheckCollision(Collider* collider1, Collider* collider2);
	static bool CheckCollisionBoxBox(Collider* box1, Collider* box2);
	static bool CheckCollisionSphereSphere(Collider* sphere1, Collider* sphere2);
	static bool CheckCollisionSphereBox(Collider* sphere, Collider* box);
};