//Resolve collisions between entities
#include "Collision.h"
#include <stdio.h> //I NEED IT

//Collisions that don't
//Make do Something
bool Collision::CheckCollision(Collider* collider1, Collider* collider2)
{
	return false;
}

//AABB-AABB Collisions
bool Collision::CheckCollisionBoxBox(Collider* box1, Collider* box2)
{
	//Divide all incoming dimensions by 2
	float box1X = box1->dimensions.x / 2;
	float box1Y = box1->dimensions.y / 2;
	float box1Z = box1->dimensions.z / 2;
	float box2X = box2->dimensions.x / 2;
	float box2Y = box2->dimensions.y / 2;
	float box2Z = box2->dimensions.z / 2;

	//Determine if there is a collision with booleans rather than if statements
	bool right1Left2Check = box1->center.x + box1X > box2->center.x - box2X; //If the right side of box 1 is colliding with the left side of box 2
	bool left1Right2Check = box1->center.x - box1X < box2->center.x + box2X; //If the left side of box 1 is colliding with the right side of box 2
	bool top1Bottom2Check = box1->center.y + box1Y > box2->center.y - box2Y; //If the top side of box 1 is colliding with the bottom side of box 2
	bool bottom1Top2Check = box1->center.y - box1Y < box2->center.y + box2Y; //If the bottom side of box 1 is colliding with the top side of box 2
	bool front1Back2Check = box1->center.z + box1Z > box2->center.z - box2Z; //If the front side of box 1 is colliding with the back side of box 2
	bool back2Front1Check = box1->center.z - box1Z < box2->center.z + box2Z; //If the back side of box 1 is colliding with the front side of box 2

	//If there 
	if (right1Left2Check && left1Right2Check && top1Bottom2Check && bottom1Top2Check && front1Back2Check && back2Front1Check)
	{
		//TODO: COLLISION RESOLUTION & REMOVING printf
		//printf("Colliding: \n");
		return true;
	}

	//TODO: COLLISION RESOLUTION & REMOVING printf
	//printf("Not Colliding: \n");
	return false;
}

//Sphere-Sphere collisions
bool Collision::CheckCollisionSphereSphere(Collider* sphere1, Collider* sphere2)
{
	//Divide all incoming dimensions by 2
	float sphere1Radius = sphere1->dimensions.x / 2;
	float sphere2Radius = sphere2->dimensions.x / 2;

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
	float sphereRadius = sphere->dimensions.x / 2;
	float boxX = box->dimensions.x / 2;
	float boxY = box->dimensions.y / 2;
	float boxZ = box->dimensions.z / 2;

	//This is a much better option than all the if statements used in the previous solution
	//https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection#Sphere_versus_AABB
	//Get the closest point from the box to the sphere by clamping
	float x = fmaxf(box->center.x - boxX, fminf(sphere->center.x, box->center.x + boxX));
	float y = fmaxf(box->center.y - boxY, fminf(sphere->center.y, box->center.y + boxY));
	float z = fmaxf(box->center.z - boxZ, fminf(sphere->center.z, box->center.z + boxZ));

	//We don't sqrt here, since using pow later is more efficient
	float distance = (float)pow(x - sphere->center.x, 2) + (float)pow(y - sphere->center.y, 2) + (float)pow(z - sphere->center.z, 2);

	//If the distance squared is greater than the sphere's radius squared, there is no collision
	if (distance > pow(sphereRadius, 2))
	{
		//TODO: COLLISION RESOLUTION & REMOVING printf
		//printf("Not Colliding: \n");
		return false;
	}

	//TODO: COLLISION RESOLUTION & REMOVING printf
	//printf("Colliding: %f\n", distance);
	return true;

}
