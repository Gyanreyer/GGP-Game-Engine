#pragma once

#include "Mesh.h"
#include "SimpleShader.h"
#include "Material.h"
#include "Transform.h"
#include "Collider.h" //Colliders

//context would be in renderer class
//ColliderType and ColliderOffset will be from Mesh
//Draw will be moved to Renderer class
class GameObject
{
public:
	GameObject();//Default constructor does nothing
	GameObject(Transform trans, Mesh * mesh, Material * material, Collider coll);
	GameObject(Transform trans, Mesh * mesh, Material * material, ColliderType colliderType);//Constructor uses a given transform so we can set that up during initialization
	GameObject(Transform trans, Collider coll);
	GameObject(Transform trans, ColliderType colliderType);//Constructor for object with no mesh
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

private:
	XMFLOAT4X4 worldMatrix;
	void UpdateWorldMatrix();

protected:
	Transform transform;//Transform determines position/rotation/scale this object is drawn at

	Mesh * mesh;//Mesh for drawing
	Material * material;//Material to apply to mesh
	Collider collider; //The collider on this object

	ID3D11DeviceContext * context; //Render target (I think)
};

