#include "GameObject.h"

GameObject::GameObject() {}

GameObject::GameObject(Mesh * mesh, Material * material, ID3D11DeviceContext * ctx)
{
	SetMesh(mesh);//Set mesh to given mesh
	SetMaterial(material);//Set material to given material

	context = ctx;

	transform = Transform();//Initialize transform
}

GameObject::~GameObject() {}

//Set given mesh to be this object's mesh and store relevant info for drawing it
void GameObject::SetMesh(Mesh * mesh)
{
	this->mesh = mesh;
	vertexBuffer = mesh->GetVertexBuffer();
	indexBuffer = mesh->GetIndexBuffer();

	meshIndexCount = mesh->GetIndexCount();
}

//Draw the mesh!
void GameObject::Draw(XMFLOAT4X4 viewMat, XMFLOAT4X4 projMat)
{
	UpdateWorldMatrix(); //Update here, removes additional method call
	PrepareMaterial(viewMat, projMat);
	context->DrawIndexed(meshIndexCount, 0, 0);
}

//Prepare material to be drawn, takes camera matrices
void GameObject::PrepareMaterial(XMFLOAT4X4 viewMat, XMFLOAT4X4 projMat)
{
	//Get shaders from material
	SimpleVertexShader * vertexShader = material->GetVertexShader();
	SimplePixelShader * pixelShader = material->GetPixelShader();

	//Set up shader data
	vertexShader->SetMatrix4x4("view", viewMat);
	vertexShader->SetMatrix4x4("projection", projMat);
	vertexShader->SetMatrix4x4("world", worldMatrix);//Set vertex shader's world matrix to this object's wm
	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetShaderResourceView("diffuseTexture",material->GetSRV());
	pixelShader->SetSamplerState("basicSampler",material->GetSampler());
	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
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
	}
}
