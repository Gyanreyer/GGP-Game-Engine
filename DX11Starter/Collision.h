//Resolve collisions between entities
#pragma once
#include "Collider.h"

class Collision
{
public:
	static bool CheckCollision(Collider* collider1, Collider* collider2);
	static bool CheckCollision(BoxCollider* box1, BoxCollider* box2);
	static bool CheckCollision(SphereCollider* sphere1, SphereCollider* sphere2);
};