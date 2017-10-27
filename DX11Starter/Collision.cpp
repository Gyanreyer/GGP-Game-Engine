//Resolve collisions between entities
#include "Collision.h"
#include <stdio.h> //I NEED IT

//Collisions that don't
bool Collision::CheckCollision(Collider* collider1, Collider* collider2)
{
	return false;
}

//AABB-AABB Collisions
bool Collision::CheckCollisionBoxBox(Collider* box1, Collider* box2)
{
	//Divide all incoming dimensions by 2
	box1->dimensions.x /= 2;
	box1->dimensions.y /= 2;
	box1->dimensions.z /= 2;
	box2->dimensions.x /= 2;
	box2->dimensions.y /= 2;
	box2->dimensions.z /= 2;

	//Determine if there is a collision with booleans rather than if statements
	bool right1Left2Check = box1->center.x + box1->dimensions.x > box2->center.x - box2->dimensions.x; //If the right side of box 1 is colliding with the left side of box 2
	bool left1Right2Check = box1->center.x - box1->dimensions.x < box2->center.x + box2->dimensions.x; //If the left side of box 1 is colliding with the right side of box 2
	bool top1Bottom2Check = box1->center.y + box1->dimensions.y > box2->center.y - box2->dimensions.y; //If the top side of box 1 is colliding with the bottom side of box 2
	bool bottom1Top2Check = box1->center.y - box1->dimensions.y < box2->center.y + box2->dimensions.y; //If the bottom side of box 1 is colliding with the top side of box 2
	bool front1Back2Check = box1->center.z + box1->dimensions.z > box2->center.z - box2->dimensions.z; //If the front side of box 1 is colliding with the back side of box 2
	bool back2Front1Check = box1->center.z - box1->dimensions.z < box2->center.z + box2->dimensions.z; //If the back side of box 1 is colliding with the front side of box 2

	//If there 
	if (right1Left2Check && left1Right2Check && top1Bottom2Check && bottom1Top2Check && front1Back2Check && back2Front1Check)
	{
		printf("Colliding: \n");
		return true;
	}

	printf("Not Colliding: \n");
	return false;
}

//Sphere-Sphere collisions
bool Collision::CheckCollisionSphereSphere(Collider* sphere1, Collider* sphere2)
{
	XMFLOAT3 pow2 = XMFLOAT3(2, 2, 2); //Vector for squaring
	XMVECTOR distanceBetweenCenter = XMLoadFloat3(&sphere1->center) - XMLoadFloat3(&sphere2->center); //Get the distance between the center of the two colliders
	float magnitude;
	XMStoreFloat(&magnitude, XMVector3Length(XMVectorSqrt(XMVectorPow(distanceBetweenCenter, XMLoadFloat3(&pow2))))); //Calculate the magnitude of the vector between the two objects

	//Calculate the combined radius
	//Incomine dimension is a diameter
	float combinedRadius = sphere1->dimensions.x / 2 + sphere2->dimensions.x / 2;

	if (magnitude > combinedRadius)  //spheres are not colliding
	{
		printf("Not Colliding: %f\n", magnitude);
		return false;
	}

	printf("Colliding: %f\n", magnitude);
	return true;
}
