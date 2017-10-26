//Resolve collisions between entities
#include "Collision.h"
#include <stdio.h>
bool Collision::CheckCollision(Collider* collider1, Collider* collider2)
{
	return false;
}

bool Collision::CheckCollision(BoxCollider* box1, BoxCollider* box2)
{
	return false;
}

bool Collision::CheckCollision(SphereCollider* sphere1, SphereCollider* sphere2)
{
	XMFLOAT3 pow2 = XMFLOAT3(2, 2, 2);
	XMVECTOR distanceBetweenCenter = XMLoadFloat3(&sphere1->center) - XMLoadFloat3(&sphere2->center);
	float magnitude;
	XMStoreFloat(&magnitude, XMVectorSqrt(XMVectorPow(distanceBetweenCenter, XMLoadFloat3(&pow2))));

	float combinedRadius = sphere1->radius + sphere2->radius;

	if (magnitude > combinedRadius)  //spheres are not colliding
	{
		
		return false;
	}

	printf("Spheres are Colliding");
	return true;
}
