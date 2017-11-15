//Resolve collisions between entities
#include "Collision.h"
#include <stdio.h> //I NEED IT

//Generic collision detection determines what to apply based on types
bool Collision::CheckCollision(Collider* collider1, Collider* collider2)
{
	if (collider1->collType == BOX && collider2->collType == BOX)
	{
		return CheckCollisionBoxBox(collider1, collider2);
	}
	else if (collider1->collType == SPHERE && collider2->collType == SPHERE)
	{
		return CheckCollisionSphereSphere(collider1, collider2);
	}
	else if (collider1->collType == SPHERE && collider2->collType == BOX)
	{
		return CheckCollisionSphereBox(collider1, collider2);
	}
	else
	{
		return CheckCollisionSphereBox(collider2, collider1);
	}
}

//AABB-AABB Collisions
bool Collision::CheckCollisionBoxBox(Collider* box1, Collider* box2)
{
	//Determine if there is a collision
	return (box1->center.x + box1->dimensions.x >= box2->center.x - box2->dimensions.x &&
		box1->center.x - box1->dimensions.x <= box2->center.x + box2->dimensions.x &&
		box1->center.y + box1->dimensions.y >= box2->center.y - box2->dimensions.y &&
		box1->center.y - box1->dimensions.y <= box2->center.y + box2->dimensions.y &&
		box1->center.z + box1->dimensions.z >= box2->center.z - box2->dimensions.z &&
		box1->center.z - box1->dimensions.z <= box2->center.z + box2->dimensions.z);
}

//Sphere-Sphere collisions
bool Collision::CheckCollisionSphereSphere(Collider* sphere1, Collider* sphere2)
{
	//Divide all incoming dimensions by 2
	float sphere1Radius = sphere1->dimensions.x;
	float sphere2Radius = sphere2->dimensions.x;

	XMFLOAT3 pow2 = XMFLOAT3(2, 2, 2); //Vector for squaring
	XMVECTOR distanceBetweenCenter = XMLoadFloat3(&sphere1->center) - XMLoadFloat3(&sphere2->center); //Get the distance between the center of the two colliders
	float magnitude;
	XMStoreFloat(&magnitude, XMVector3Length(XMVectorSqrt(XMVectorPow(distanceBetweenCenter, XMLoadFloat3(&pow2))))); //Calculate the magnitude of the vector between the two objects

	//Calculate the combined radius
	//Incomine dimension is a diameter
	float combinedRadius = sphere1Radius + sphere2Radius;

	if (magnitude > combinedRadius)  //spheres are not colliding
	{
		//TODO: COLLISION RESOLUTION & REMOVING printf
		//printf("Not Colliding: %f\n", magnitude);
		return false;
	}

	//TODO: COLLISION RESOLUTION & REMOVING printf
	//printf("Colliding: %f\n", magnitude);
	return true;
}

bool Collision::CheckCollisionSphereBox(Collider * sphere, Collider * box)
{
	//Divide all incoming dimensions by 2
	float sphereRadius = sphere->dimensions.x;

	//This is a much better option than all the if statements used in the previous solution
	//https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection#Sphere_versus_AABB
	//Get the closest point from the box to the sphere by clamping
	float x = fmaxf(box->center.x - box->dimensions.x, fminf(sphere->center.x, box->center.x + box->dimensions.x));
	float y = fmaxf(box->center.y - box->dimensions.y, fminf(sphere->center.y, box->center.y + box->dimensions.y));
	float z = fmaxf(box->center.z - box->dimensions.z, fminf(sphere->center.z, box->center.z + box->dimensions.z));

	//We don't sqrt here, since using pow later is more efficient
	//If the distance squared is greater than the sphere's radius squared, there is no collision
	return (float)pow(x - sphere->center.x, 2) + (float)pow(y - sphere->center.y, 2) + (float)pow(z - sphere->center.z, 2) >
		pow(sphereRadius, 2);
}
