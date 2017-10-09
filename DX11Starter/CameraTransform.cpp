#include "CameraTransform.h"

CameraTransform::CameraTransform() :
	CameraTransform(XMFLOAT3(0,0,-5),XMFLOAT3(0,0,0))
{
}

CameraTransform::CameraTransform(XMFLOAT3 pos, XMFLOAT3 rot) :
	Transform(pos,rot)
{
}

CameraTransform::~CameraTransform()
{
}

XMFLOAT4X4 CameraTransform::GetViewMatrixFloat()
{
	UpdateViewMatrix();
	return viewMatrix;
}

XMMATRIX CameraTransform::GetViewMatrixXMMat()
{
	UpdateViewMatrix();
	return XMLoadFloat4x4(&viewMatrix);
}

void CameraTransform::UpdateViewMatrix()
{
	//If matrix needs update, update it with new pos and forward vec
	if (matrixNeedsUpdate) {
		XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMMatrixLookToLH(GetPosition(),GetForward(), UP)));
	}
}
