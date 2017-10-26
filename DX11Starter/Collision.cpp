//Resolve collisions between entities
#include "Collision.h"

bool Collision::CheckCollision(Collider* collider1, Collider* collider2)
{
	return false;
}

bool Collision::CheckCollisionBoxBox(Collider* box1, Collider* box2)
{
	return false;
}

#include <stdio.h>
bool Collision::CheckCollisionSphereSphere(Collider* sphere1, Collider* sphere2)
{
	XMFLOAT3 pow2 = XMFLOAT3(2, 2, 2);
	XMVECTOR distanceBetweenCenter = XMLoadFloat3(&sphere1->center) - XMLoadFloat3(&sphere2->center);
	float magnitude;
	XMStoreFloat(&magnitude, XMVector3Length(XMVectorSqrt(XMVectorPow(distanceBetweenCenter, XMLoadFloat3(&pow2)))));

	float combinedRadius = sphere1->dimensions.x / 2 + sphere2->dimensions.x / 2;

	if (magnitude > combinedRadius)  //spheres are not colliding
	{
		printf("Not Colliding: %f\n", magnitude);
		return false;
	}

	printf("Colliding: %f\n", magnitude);
	return true;
}
