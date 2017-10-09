#include "Transform.h"

Transform::Transform() : Transform(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0))
{
}

Transform::Transform(XMFLOAT3 pos, XMFLOAT3 rot)
{
	SetPosition(pos);
	SetRotation(rot);

	parent = nullptr;
}

Transform::~Transform()
{
}

void Transform::SetPosition(float x, float y, float z)
{
	SetPosition(XMVectorSet(x, y, z, 0.0f));
}

void Transform::SetPosition(XMFLOAT3 pos)
{
	SetPosition(XMLoadFloat3(&pos));
}

void Transform::SetPosition(XMVECTOR pos)
{
	XMStoreFloat3(&position, pos);
	NotifyMatrixUpdate();//Need to update world matrix now that pos changed
}

void Transform::SetRotation(float roll, float pitch, float yaw)
{
	SetRotation(XMVectorSet(roll, pitch, yaw, 0.0f));
}

void Transform::SetRotation(XMFLOAT3 rot)
{
	SetRotation(XMLoadFloat3(&rot));
}

void Transform::SetRotation(XMVECTOR rot)
{
	XMStoreFloat3(&rotation, rot);
	UpdateDirectionVectors();//Update direction vectors for new rotation
	NotifyMatrixUpdate();
}

void Transform::Move(float x, float y, float z)
{
	Move(XMVectorSet(x,y,z,0));
}

void Transform::Move(XMVECTOR moveVec)
{
	SetPosition(GetPosition() + moveVec);
}

void Transform::Rotate(float roll, float pitch, float yaw)
{
	SetRotation(GetRotation() + XMVectorSet(roll, pitch, yaw, 0.0f));
}

void Transform::MoveRelative(float fwdSpeed, float sideSpeed, float upSpeed)
{
	if (fwdSpeed == 0.0f && sideSpeed == 0.0f && upSpeed == 0.0f) return;//Return early if no movement

	Move(GetForward() * fwdSpeed + GetRight() * sideSpeed + GetUp() * upSpeed);
}

void Transform::MoveRelativeAxes(float fwdSpeed, float sideSpeed, float upSpeed)
{
	if (fwdSpeed == 0.0f && sideSpeed == 0.0f && upSpeed == 0.0f) return;//Return early if no movement

	Move(GetForwardXZ() * fwdSpeed + GetRight() * sideSpeed + UP * upSpeed);
}

XMVECTOR Transform::GetPosition()
{
	return XMLoadFloat3(&position);
}

XMVECTOR Transform::GetRotation()
{
	return XMLoadFloat3(&rotation);
}

XMVECTOR Transform::GetForward()
{
	return XMLoadFloat3(&forward);
}

XMVECTOR Transform::GetForwardXZ()
{
	return XMVector3Normalize(XMVectorSet(1,0,1,0)*GetForward());
}

XMVECTOR Transform::GetRight()
{
	return XMLoadFloat3(&right);
}

XMVECTOR Transform::GetUp()
{
	return XMLoadFloat3(&up);
}

XMMATRIX Transform::GetPositionMatrix()
{
	return XMMatrixTranslationFromVector(GetPosition());
}

XMMATRIX Transform::GetRotationMatrix()
{
	return XMMatrixRotationRollPitchYawFromVector(GetRotation());
}

void Transform::UpdateDirectionVectors()
{
	//Update forward vector by transforming forward vec w/ rotation matrix calculated from x/y rotation
	XMStoreFloat3(&forward, XMVector3TransformNormal(FWD, GetRotationMatrix()));
	
	//Update right vector by crossing forward vec w/ UP const
	XMStoreFloat3(&right, XMVector3Normalize(XMVector3Cross(GetForward(), UP)));

	//Update up vector by crossing forward w/ right
	XMStoreFloat3(&up, XMVector3Normalize(XMVector3Cross(GetRight(), GetForward())));
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


