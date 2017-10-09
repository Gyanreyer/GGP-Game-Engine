#pragma once
#include "Transform.h"

class CameraTransform :
	public Transform
{
public:
	CameraTransform();//Default constructor sets pos at 0,0,-5 and rot at 0,0,0
	CameraTransform(XMFLOAT3 pos, XMFLOAT3 rot);//Constructor takes pos and rotation, passes to Transform constructor
	~CameraTransform();

	XMMATRIX GetViewMatrixXMMat();
	XMFLOAT4X4 GetViewMatrixFloat();

private:
	XMFLOAT4X4 viewMatrix;

	void UpdateViewMatrix();
};

