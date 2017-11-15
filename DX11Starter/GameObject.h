#pragma once

#include "Mesh.h"
#include "SimpleShader.h"
#include "Material.h"
#include "Transform.h"
#include "Collider.h" //Colliders


class OctreeNode;//Resolves circular dependency
class GameObject
{
public:
	GameObject();//Default constructor does nothing
	GameObject(Transform trans, Mesh * mesh, Material * material, char* objTag);
	GameObject(Transform trans, ColliderType colliderType, char* objTag);
	~GameObject();

	//Takes a Mesh pointer to use for drawing this object
	void SetMesh(Mesh* mesh);

	//Draws object's mesh with given pos/rot/scale
	//void Draw(XMFLOAT4X4 viewMat, XMFLOAT4X4 projMat);

	//Prepare material for drawing, called in Draw
	//void PrepareMaterial(XMFLOAT4X4 viewMat, XMFLOAT4X4 projMat);

	void SetMaterial(Material * newMat);

	Transform* GetTransform();//Get reference to this object's transform
	Material* GetMaterial(); //Get objects Material
	Mesh* GetMesh();
	Collider* GetCollider();
	XMFLOAT4X4 GetWorldMatrix();

	OctreeNode * GetOctNode();
	void SetOctNode(OctreeNode * newOct);

	char * GetTag();

private:
	XMFLOAT4X4 worldMatrix;
	void UpdateWorldMatrix();

protected:
	Transform transform;//Transform determines position/rotation/scale this object is drawn at

	Mesh * mesh;//Mesh for drawing
	Material * material;//Material to apply to mesh

	Collider collider; //The collider on this object

	OctreeNode * currentOctNode;

	char * tag;
};

