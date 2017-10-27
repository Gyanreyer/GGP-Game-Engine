//Resolve collisions between entities
#include "Collision.h"
#include <stdio.h> //I NEED IT
bool Collision::CheckCollision(Collider* collider1, Collider* collider2)
{
	return false;
}

bool Collision::CheckCollisionBoxBox(Collider* box1, Collider* box2)
{

	bool right1Left2Check = box1->center.x + box1->dimensions.x > box2->center.x - box2->dimensions.x; //If the right side of box 1 is colliding with the left side of box 2
	bool left1Right2Check = box1->center.x - box1->dimensions.x < box2->center.x + box2->dimensions.x; //If the left side of box 1 is colliding with the right side of box 2
	bool top1Bottom2Check = box1->center.y + box1->dimensions.y > box2->center.y - box2->dimensions.y; //If the top side of box 1 is colliding with the bottom side of box 2
	bool bottom1Top2Check = box1->center.y - box1->dimensions.y < box2->center.y + box2->dimensions.y; //If the bottom side of box 1 is colliding with the top side of box 2
	bool front1Back2Check = box1->center.z + box1->dimensions.z > box2->center.z - box2->dimensions.z; //If the front side of box 1 is colliding with the back side of box 2
	bool back2Front1Check = box1->center.z - box1->dimensions.z < box2->center.z + box2->dimensions.z; //If the back side of box 1 is colliding with the front side of box 2

	if (right1Left2Check && left1Right2Check && top1Bottom2Check && bottom1Top2Check && front1Back2Check && back2Front1Check)
	{
		printf("Colliding: \n");
		return true;
	}

	printf("Not Colliding: \n");
	return false;
}

bool Collision::CheckCollisionSphereSphere(Collider* sphere1, Collider* sphere2)
{
	XMFLOAT3 pow2 = XMFLOAT3(2, 2, 2);
	XMVECTOR distanceBetweenCenter = XMLoadFloat3(&sphere1->center) - XMLoadFloat3(&sphere2->center);
	float magnitude;
	XMStoreFloat(&magnitude, XMVector3Length(XMVectorSqrt(XMVectorPow(distanceBetweenCenter, XMLoadFloat3(&pow2)))));

	float combinedRadius = sphere1->dimensions.x + sphere2->dimensions.x;

	if (magnitude > combinedRadius)  //spheres are not colliding
	{
		printf("Not Colliding: %f\n", magnitude);
		return false;
	}

	printf("Colliding: %f\n", magnitude);
	return true;
}
