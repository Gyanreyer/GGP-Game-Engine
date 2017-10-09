#pragma once

#include "Mesh.h"
#include "SimpleShader.h"
#include "Material.h"
#include "ObjectTransform.h"

class GameObject
{
public:
	GameObject();//Default constructor does nothing
	GameObject(Mesh * mesh, Material * material, ID3D11DeviceContext * context);//Constructor sets mesh/material/context for drawing and initializes transform
	~GameObject();

	//Takes a Mesh pointer to use for drawing this object
	void SetMesh(Mesh* mesh);

	//Draws object's mesh with given pos/rot/scale
	void Draw(XMFLOAT4X4 viewMat, XMFLOAT4X4 projMat);

	//Prepare material for drawing, called in Draw
	void PrepareMaterial(XMFLOAT4X4 viewMat, XMFLOAT4X4 projMat);

	void SetMaterial(Material * newMat);

	ObjectTransform * GetTransform();//Get reference to this object's transform
	Material* GetMaterial(); //Get objects Material

private:
	Mesh * mesh;//Mesh for drawing
	Material * material;//Material to apply to mesh

	//Buffers and index count for drawing mesh
	ID3D11Buffer * vertexBuffer;
	ID3D11Buffer * indexBuffer;
	int meshIndexCount;

	ID3D11DeviceContext * context;

	ObjectTransform transform;//Transform determines position/rotation/scale this object is drawn at
};

