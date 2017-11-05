#include "GameObject.h"

GameObject::GameObject() {}

//GameObject without collider
GameObject::GameObject(Mesh * mesh, Material * material, ID3D11DeviceContext * ctx)
{
	SetMesh(mesh);//Set mesh to given mesh
	SetMaterial(material);//Set material to given material

	coll = Collider();
	context = ctx;

	transform = Transform();//Initialize transform
}

//GameObject with collider
GameObject::GameObject(Mesh * mesh, Material * material, ColliderType colliderType, bool isColliderOffset, ID3D11DeviceContext * ctx)
{
	SetMesh(mesh);//Set mesh to given mesh
	SetMaterial(material);//Set material to given material

	transform = Transform();//Initialize transform

	//Determine whether or not the collider is offset
	if (!isColliderOffset)
		coll = Collider(colliderType, transform.GetPosition(), transform.GetScale(), false);
	else
		coll = Collider(colliderType, transform.GetPosition(), transform.GetScale(), false, true);

	context = ctx;
}

//Just a collider, no visible object
GameObject::GameObject(ColliderType colliderType)
{
	transform = Transform();

	//This collider type will never be offset
	coll = Collider(colliderType, transform.GetPosition(), transform.GetScale(), false);

	hasMesh = false;//This object doesn't have a mesh to be drawn
}

GameObject::~GameObject() {}

//Set given mesh to be this object's mesh and store relevant info for drawing it
void GameObject::SetMesh(Mesh * mesh)
{
	this->mesh = mesh;
	vertexBuffer = mesh->GetVertexBuffer();
	indexBuffer = mesh->GetIndexBuffer();

	meshIndexCount = mesh->GetIndexCount();

	hasMesh = true;
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
	return &coll;
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

		coll.dimensions = transform.GetScale();

		//Check if the collider is offset
		if (!coll.isOffset)
			//If the collider is not offset, proceed as normal
			coll.center = transform.GetPosition();
		else
			//Adjust for offset here
			//Right now, this assumes that the collider is at the "feet" of a model
			//If the need arises, this can be generalized
			coll.center = XMFLOAT3(transform.GetPosition().x, transform.GetPosition().y + (coll.dimensions.y / 2), transform.GetPosition().z);
	}
}
