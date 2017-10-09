#include "ObjectTransform.h"



ObjectTransform::ObjectTransform() :
	ObjectTransform(XMFLOAT3(0,0,0), XMFLOAT3(0,0,0), XMFLOAT3(1,1,1))
{
}

ObjectTransform::ObjectTransform(XMFLOAT3 pos, XMFLOAT3 rot, float scalar) :
	ObjectTransform(pos,rot,XMFLOAT3(scalar,scalar,scalar))
{
}

ObjectTransform::ObjectTransform(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale) :
	Transform(pos, rot)
{
	SetScale(scale);
}

ObjectTransform::~ObjectTransform()
{
}

void ObjectTransform::SetScale(float scalar)
{
	SetScale(scalar, scalar, scalar);
}

void ObjectTransform::SetScale(float x, float y, float z)
{
	SetScale(XMVectorSet(x, y, z, 0));
}

void ObjectTransform::SetScale(XMFLOAT3 scl)
{
	SetScale(XMLoadFloat3(&scl));
}

void ObjectTransform::SetScale(XMVECTOR scl)
{
	XMStoreFloat3(&scale, scl);
	NotifyMatrixUpdate();
}

void ObjectTransform::SetWorldMatrix(XMMATRIX wm)
{
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(wm));
}

XMVECTOR ObjectTransform::GetScale()
{
	return XMLoadFloat3(&scale);
}

XMMATRIX ObjectTransform::GetScaleMatrix()
{
	return XMMatrixScalingFromVector(GetScale());
}

XMMATRIX ObjectTransform::GetWorldMatrixXMMat()
{
	UpdateWorldMatrix();

	return XMLoadFloat4x4(&worldMatrix);
}

XMFLOAT4X4 ObjectTransform::GetWorldMatrixFloat()
{
	UpdateWorldMatrix();

	return worldMatrix;
}

void ObjectTransform::UpdateWorldMatrix()
{
	//If something was updated since last get call, update the world matrix to reflect that
	if (matrixNeedsUpdate) {

		//If this transform has a parent then factor that parent's translation and rotation into new matrix
		if (GetParent())
			SetWorldMatrix(GetScaleMatrix() * GetRotationMatrix() * GetPositionMatrix() * GetParent()->GetRotationMatrix() * GetParent()->GetPositionMatrix());
		else
			SetWorldMatrix(GetScaleMatrix() * GetRotationMatrix() * GetPositionMatrix());

		matrixNeedsUpdate = false;//Done updating matrix
	}
}