//Colliders for entities
#pragma once

enum ColliderType {
BOX,
SPHERE,
MESH, //probably will never implement
NONE
};

class Collider
{
public:
	Collider(); //Constructor
	~Collider(); //Destructor
	bool CheckCollision(Collider objCollider);


private:
	float width; //Collider Width
	float height;

};
