#include "Transform.h"

Transform::Transform() : Transform(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1))
{
}

Transform::Transform(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale)
{
	SetPosition(pos);
	SetRotation(rot);
	SetScale(scale);

	parent = nullptr;
}

Transform::~Transform()
{
}

void Transform::SetPosition(float x, float y, float z)
{
	SetPosition(XMFLOAT3(x, y, z));
}

void Transform::SetPosition(XMFLOAT3 pos)
{
	position = pos;
	NotifyMatrixUpdate();
}

void Transform::SetPosition(XMVECTOR pos)
{
	XMStoreFloat3(&position, pos);
	NotifyMatrixUpdate();//Need to update world matrix now that pos changed
}

void Transform::SetRotation(float roll, float pitch, float yaw)
{
	SetRotation(XMFLOAT3(roll, pitch, yaw));
}

void Transform::SetRotation(XMFLOAT3 rot)
{
	rotation = rot;
	UpdateDirectionVectors();
	NotifyMatrixUpdate();
}

void Transform::SetRotation(XMVECTOR rot)
{
	XMStoreFloat3(&rotation, rot);
	UpdateDirectionVectors();//Update direction vectors for new rotation
	NotifyMatrixUpdate();
}

void Transform::SetScale(float scalar)
{
	SetScale(XMFLOAT3(scalar,scalar,scalar));
}

void Transform::SetScale(float x, float y, float z)
{
	SetScale(XMFLOAT3(x,y,z));
}

void Transform::SetScale(XMFLOAT3 scl)
{
	scale = scl;
	NotifyMatrixUpdate();
}

void Transform::SetScale(XMVECTOR scl)
{
	XMStoreFloat3(&scale,scl);
	NotifyMatrixUpdate();
}

void Transform::Move(float x, float y, float z)
{
	Move(XMVectorSet(x,y,z,0));
}

void Transform::Move(XMFLOAT3 moveVec)
{
	Move(XMLoadFloat3(&moveVec));
}

void Transform::Move(XMVECTOR moveVec)
{
	SetPosition(XMLoadFloat3(&position) + moveVec);
}

void Transform::Rotate(float roll, float pitch, float yaw)
{
	SetRotation(XMLoadFloat3(&rotation) + XMVectorSet(roll, pitch, yaw, 0.0f));
}

void Transform::MoveRelative(float fwdSpeed, float sideSpeed, float upSpeed)
{
	if (fwdSpeed == 0.0f && sideSpeed == 0.0f && upSpeed == 0.0f) return;//Return early if no movement

	Move(XMLoadFloat3(&forward) * fwdSpeed + XMLoadFloat3(&right) * sideSpeed + XMLoadFloat3(&up) * upSpeed);
}

void Transform::MoveRelativeAxes(float fwdSpeed, float sideSpeed, float upSpeed)
{
	if (fwdSpeed == 0.0f && sideSpeed == 0.0f && upSpeed == 0.0f) return;//Return early if no movement

	Move(XMLoadFloat3(&GetForwardXZ()) * fwdSpeed + XMLoadFloat3(&right) * sideSpeed + UP * upSpeed);
}

XMFLOAT3 Transform::GetPosition()
{
	return position;
}

XMFLOAT3 Transform::GetRotation()
{
	return rotation;
}

XMFLOAT3 Transform::GetScale()
{
	return scale;
}

XMFLOAT3 Transform::GetForward()
{
	return forward;
}

XMFLOAT3 Transform::GetForwardXZ()
{
	XMFLOAT3 fxz;

	XMStoreFloat3(&fxz,XMVector3Normalize(XMVectorSet(1, 0, 1, 0)*XMLoadFloat3(&forward)));

	return fxz;
}

XMFLOAT3 Transform::GetRight()
{
	return right;
}

XMFLOAT3 Transform::GetUp()
{
	return up;
}

bool Transform::MatrixNeedsUpdate()
{
	return matrixNeedsUpdate;
}

void Transform::DoneUpdating()
{
	matrixNeedsUpdate = false;
}

void Transform::UpdateDirectionVectors()
{
	//Update forward vector by transforming forward vec w/ rotation matrix calculated from x/y rotation
	XMStoreFloat3(&forward, XMVector3TransformNormal(FWD, XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rotation))));
	
	//Update right vector by crossing forward vec w/ UP const
	XMStoreFloat3(&right, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&forward), UP)));

	//Update up vector by crossing forward w/ right
	XMStoreFloat3(&up, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&right), XMLoadFloat3(&forward))));
}

//Set a parent transform for this transform which it will center its position/rotation around
void Transform::SetParent(Transform * parent)
{
	if (this->parent) RemoveParent();//If transform already has a parent, remove it first before setting new one

	StoreParent(parent);//Store parent
	parent->StoreChild(this);//Add this transform to parent's children
}

//Remove parent of this transform
void Transform::RemoveParent()
{
	//If parent exists, remove this from parent's children and set parent to nullptr
	if (parent) {
		parent->RemoveChild(this);
		RemoveParentReference();
	}
}

//Get reference to parent
Transform * Transform::GetParent()
{
	return parent;
}

//Add a child to this transform
void Transform::AddChild(Transform * child)
{
	child->RemoveParent();//Remove existing parent from child if it has one

	StoreChild(child);//Store child in children
	child->StoreParent(this);//Store reference to this as child's parent
}

//Remove a child from child list at given index
void Transform::RemoveChild(int index)
{
	//Remove the child's reference to this as a parent and then remove from children
	if (index < GetChildCount())
	{
		children[index]->RemoveParentReference();
		RemoveChildReference(index);
	}
}

//Search for a transform in children and remove it
void Transform::RemoveChild(Transform * child)
{
	//Loop through children
	for (int i = 0; i < GetChildCount(); i++) {
		if (children[i] == child) {//If we hit a match, remove their references to each other and return early
			child->RemoveParentReference();
			RemoveChildReference(i);
			return;
		}
	}
}

//Return reference to child at given index, returns nullptr if index invalid
Transform * Transform::GetChild(int index)
{
	if (GetChildCount() > index) {
		return children[index];
	}
	return nullptr;
}

//Get number of children that transform has
int Transform::GetChildCount()
{
	return children.size();
}

//Store reference to parent - SHOULD ONLY BE USED IN CONJUNCTION WITH StoreChild()
void Transform::StoreParent(Transform * parent)
{
	this->parent = parent;
}

//Add child to children vector - SHOULD ONLY BE USED IN CONJUNCTION WITH StoreParent()
void Transform::StoreChild(Transform * child)
{
	children.push_back(child);
}

//Remove parent - SHOULD ONLY BE USED IN CONJUNCTION WITH RemoveChildReference()
void Transform::RemoveParentReference()
{
	if(parent)
		parent = nullptr;
}

//Remove child at index - SHOULD ONLY BE USED IN CONJUNCTION WITH RemoveParentReference()
void Transform::RemoveChildReference(int index)
{
	if(index < GetChildCount())
		children.erase(children.begin() + index);
}

//Call to notify transform that matrix needs to be updated
void Transform::NotifyMatrixUpdate()
{
	matrixNeedsUpdate = true;

	//Loop through all children and notify them that their world matrix needs to be updated too
	for (int i = 0; i < GetChildCount(); i++) {
		children[i]->NotifyMatrixUpdate();
	}
}


