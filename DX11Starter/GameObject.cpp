#include "GameObject.h"

GameObject::GameObject() {}

GameObject::GameObject(Transform trans, Mesh * mes, Material * material, Collider coll)
{
	transform = trans;

	SetMesh(mesh);
	SetMaterial(material);

	collider = coll;
}

GameObject::GameObject(Transform trans, Mesh * mesh, Material * material, ColliderType colliderType)
{
	transform = trans;

	SetMesh(mesh);
	SetMaterial(material);

	collider = Collider(colliderType, &transform);
}

GameObject::GameObject(Transform trans, Collider coll)
{
	transform = trans;
	collider = coll;
}

//Just a collider, no visible object
GameObject::GameObject(Transform trans, ColliderType colliderType)
{
	transform = trans;

	//This collider type will never be offset
	collider = Collider(colliderType, &transform);
}

GameObject::~GameObject() {}

//Set given mesh to be this object's mesh and store relevant info for drawing it
void GameObject::SetMesh(Mesh * mesh)
{
	this->mesh = mesh;
}

void GameObject::SetMaterial(Material * newMat)
{
	material = newMat;
}

Transform * GameObject::GetTransform()
{
	return &transform;
}

Material * GameObject::GetMaterial()
{
	return material;
}

Mesh * GameObject::GetMesh()
{
	return mesh;
}

Collider* GameObject::GetCollider()
{
	return &collider;
}

XMFLOAT4X4 GameObject::GetWorldMatrix()
{
	UpdateWorldMatrix();//Make sure world matrix is up to date before returning
	return worldMatrix;
}

//Update the world matrix if transform has changed
void GameObject::UpdateWorldMatrix()
{
	if (transform.MatrixNeedsUpdate()) {
		//World matrix from scale, rot, and pos of transform
		XMMATRIX newWM = XMMatrixScalingFromVector(XMLoadFloat3(&transform.GetScale())) *
			XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&transform.GetRotation())) *
			XMMatrixTranslationFromVector(XMLoadFloat3(&transform.GetPosition()));

		//If transform has a parent, apply its rot and pos to matrix too
		if (transform.GetParent()) {
			newWM *= XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&transform.GetParent()->GetRotation())) *
				XMMatrixTranslationFromVector(XMLoadFloat3(&transform.GetParent()->GetPosition()));
		}		

		//Store transposed matrix as worldMatrix
		XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(newWM));

		transform.DoneUpdating();//Notify transform that matrix has been updated successfully

		collider.dimensions = transform.GetScale();
		collider.center = transform.GetPosition();

		//Check if the collider is offset
		if (collider.isOffset)
			//Adjust for offset here
			//Right now, this assumes that the collider is at the "feet" of a model
			//If the need arises, this can be generalized
			collider.center.y += collider.dimensions.y / 2;			
	}
}
