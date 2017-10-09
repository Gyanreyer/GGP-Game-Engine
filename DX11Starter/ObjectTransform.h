#pragma once
#include "Transform.h"

class ObjectTransform :
	public Transform
{
public:
	ObjectTransform();//Default constructor sets pos and rot at 0,0,0, scale at 1,1,1
	ObjectTransform(XMFLOAT3 pos, XMFLOAT3 rot, float scalar);//Constructor takes pos, rotation, and scalar for all scale dimensions
	ObjectTransform(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale);//Constructor sets pos, rot, and scale and calls Transform constructor
	~ObjectTransform();

	//Sets scale of object, overloaded so may pass in scale in many different ways
	void SetScale(float scalar);
	void SetScale(float x, float y, float z);
	void SetScale(XMFLOAT3 scl);
	void SetScale(XMVECTOR scl);

	//Getters for scale vec and mat
	XMVECTOR GetScale();
	XMMATRIX GetScaleMatrix();

	//Sets world matrix of object given an XMMATRIX
	void SetWorldMatrix(XMMATRIX wm);

	//Getter methods update WM if necessary before returning as either XMMATRIX or XMFLOAT4X4
	XMMATRIX GetWorldMatrixXMMat();
	XMFLOAT4X4 GetWorldMatrixFloat();

private:
	XMFLOAT3 scale;//Scale of object

	XMFLOAT4X4 worldMatrix;//World matrix for drawing

	void UpdateWorldMatrix();//Update the world matrix, should be used after any changes to pos, rot, or scale
};

