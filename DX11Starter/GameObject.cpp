#include "GameObject.h"


GameObject::GameObject() {}

GameObject::GameObject(Mesh * mesh, Material * material, ID3D11DeviceContext * context)
{
	SetMesh(mesh);//Set mesh to given mesh
	SetMaterial(material);//Set material to given material

	this->context = context;

	transform = ObjectTransform();//Initialize transform
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
	PrepareMaterial(viewMat, projMat);
	context->DrawIndexed(meshIndexCount, 0, 0);
}

//Prepare material to be drawn, takes camera matrices
void GameObject::PrepareMaterial(XMFLOAT4X4 viewMat, XMFLOAT4X4 projMat)
{
	//Get shaders from material
	SimpleVertexShader * vertexShader = material->GetVertexShader();
	SimplePixelShader * pixelShader = material->GetPixelShader();

	XMFLOAT4X4 test;
	XMStoreFloat4x4(&test, XMMatrixIdentity());

	//Set up shader data
	vertexShader->SetMatrix4x4("view", viewMat);
	vertexShader->SetMatrix4x4("projection", projMat);
	vertexShader->SetMatrix4x4("world", transform.GetWorldMatrixFloat());//Set vertex shader's world matrix to this object's wm
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

ObjectTransform * GameObject::GetTransform()
{
	return &transform;
}
