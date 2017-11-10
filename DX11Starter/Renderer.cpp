#include "Renderer.h"

Renderer::Renderer(DirectX::XMFLOAT4X4 viewMat, DirectX::XMFLOAT4X4 projectMat, ID3D11DeviceContext * context, ID3D11Device * device)
{
	viewMatrix = viewMat;
	projectionMatrix = projectMat;
	this->device = device;
	this->context = context;

	CreateLights();
}

Renderer::~Renderer()
{
}

void Renderer::CreateLights()
{
	//Create directional lights
	DirectionalLight light1 = { XMFLOAT4(0.1f,0.1f,0.1f,1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f),XMFLOAT3(1.0f,-1.0f,0.5f) };
	DirectionalLight light2 = { XMFLOAT4(0,0,0,0),XMFLOAT4(1.0f,0.0f,0.0f,1.0f),XMFLOAT3(-0.5f,-0.5f,0.25f) };

	directionalLights.push_back(light1);
	directionalLights.push_back(light2);
}

void Renderer::SetViewProjMatrix(DirectX::XMFLOAT4X4 viewMat, DirectX::XMFLOAT4X4 projectMat)
{
	viewMatrix = viewMat;
	projectionMatrix = projectMat;
}

void Renderer::Render(GameObject * gameObject)
{	
	gameObject->GetMaterial()->GetPixelShader()->SetData(
		"light1",
		&directionalLights[0],
		sizeof(DirectionalLight));

	gameObject->GetMaterial()->GetPixelShader()->SetData(
		"light2",
		&directionalLights[1],
		sizeof(DirectionalLight));

	//Prepare Material
	//Get shaders from material
	SimpleVertexShader * vertexShader = gameObject->GetMaterial()->GetVertexShader();
	SimplePixelShader * pixelShader = gameObject->GetMaterial()->GetPixelShader();

	//Set up shader data
	vertexShader->SetMatrix4x4("view", viewMatrix);
	vertexShader->SetMatrix4x4("projection", projectionMatrix);
	vertexShader->SetMatrix4x4("world", gameObject->GetWorldMatrix());//Set vertex shader's world matrix to this object's wm
	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	pixelShader->SetShaderResourceView("diffuseTexture", gameObject->GetMaterial()->GetSRV());
	pixelShader->SetSamplerState("basicSampler", gameObject->GetMaterial()->GetSampler());
	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	ID3D11Buffer* vertexBuffer = gameObject->GetMesh()->GetVertexBuffer();
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(gameObject->GetMesh()->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(gameObject->GetMesh()->GetIndexCount(), 0, 0);
}
