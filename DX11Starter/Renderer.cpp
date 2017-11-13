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
	//Ambient
	//Diffuse
	//Direction
	DirectionalLight dLight1 = { XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, -1.0f, 0.5f) };
	DirectionalLight dLight2 = { XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(-0.5f, -0.5f, 0.25f) };
	directionalLights.push_back(dLight1);
	directionalLights.push_back(dLight2);

	//Point lights
	//Ambient
	//Diffuse
	//Position
	PointLight pLight1 = { XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT3(-0.5f, -0.5f, -0.5f) };
	pointLights.push_back(pLight1);
}

void Renderer::SetViewProjMatrix(DirectX::XMFLOAT4X4 viewMat, DirectX::XMFLOAT4X4 projectMat)
{
	viewMatrix = viewMat;
	projectionMatrix = projectMat;
}

void Renderer::Render(GameObject * gameObject)
{
	//Prepare Material
	//Get shaders from material
	Material* goMaterial = gameObject->GetMaterial(); //Get the material from this object
	SimpleVertexShader * vertexShader = goMaterial->GetVertexShader();
	SimplePixelShader * pixelShader = goMaterial->GetPixelShader();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	Mesh* goMesh = gameObject->GetMesh(); //Get the mesh from this object
	ID3D11Buffer* vertexBuffer = goMesh->GetVertexBuffer();
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(goMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	//Set up vertex shader data
	vertexShader->SetMatrix4x4("world", gameObject->GetWorldMatrix()); //Set vertex shader's world matrix to this object's wm
	vertexShader->SetMatrix4x4("view", viewMatrix);
	vertexShader->SetMatrix4x4("projection", projectionMatrix);
	vertexShader->CopyAllBufferData();
	vertexShader->SetShader();

	//Set up pixel shader data
	pixelShader->SetData(
		"dLight1",
		&directionalLights[0],
		sizeof(DirectionalLight));

	pixelShader->SetData(
		"dLight2",
		&directionalLights[1],
		sizeof(DirectionalLight));

	pixelShader->SetData(
		"pLight1",
		&pointLights[0],
		sizeof(PointLight));

	pixelShader->SetShaderResourceView("diffuseTexture", goMaterial->GetSRV()); //Texture
	pixelShader->SetShaderResourceView("normalMap", goMaterial->GetNormalSRV()); //Normal map, not every object has one right now, but they all probably should
	pixelShader->SetSamplerState("basicSampler", goMaterial->GetSampler()); //Sampler
	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();

	context->DrawIndexed(goMesh->GetIndexCount(), 0, 0);
}
