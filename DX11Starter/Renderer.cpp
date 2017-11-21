#include "Renderer.h"

Renderer::Renderer(DirectX::XMFLOAT4X4 viewMat, DirectX::XMFLOAT4X4 projectMat, ID3D11DeviceContext * context, ID3D11Device * device, Player* play)
{
	player = play;

	viewMatrix = viewMat;
	projectionMatrix = projectMat;
	this->device = device;
	this->context = context;

	CreateLights();

	// A depth state for the particles
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // Turns off depth writing
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	device->CreateDepthStencilState(&dsDesc, &particleDepthState);


	// Blend for particles (additive)
	D3D11_BLEND_DESC blend = {};
	blend.AlphaToCoverageEnable = false;
	blend.IndependentBlendEnable = false;
	blend.RenderTarget[0].BlendEnable = true;
	blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blend, &particleBlendState);
}

Renderer::~Renderer()
{
	particleBlendState->Release();
	particleDepthState->Release();
}

void Renderer::CreateLights()
{
	//AMBIENT LIGHT IS PASSED SEPARATELY INTO THE SHADER
	//IT'S MORE SIMPLE THAN MANAGING IT VIA ALL OF OUR LIGHTS

	//Create directional lights
	//Diffuse
	//Direction
	DirectionalLight dLight1 = { XMFLOAT4(.4f, .4f, .4f, 1.0f), XMFLOAT3(-10.0f, -10.0f, -4.0f) };
	directionalLights.push_back(dLight1);

	//Point lights
	//Diffuse
	//Position
	PointLight pLight1 = { XMFLOAT4(.6f, .6f, 0.0f, 1.0f), XMFLOAT3(3.225f, 1.65f, 0) };
	PointLight pLight2 = { XMFLOAT4(.6f, .6f, 0.0f, 1.0f), XMFLOAT3(-4.725f, 1.65f, -3.5f) };
	pointLights.push_back(pLight1);
	pointLights.push_back(pLight2);
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
	//Send light data
	pixelShader->SetData(
		"dLight1",
		&directionalLights[0],
		sizeof(DirectionalLight));

	pixelShader->SetData(
		"pLight1",
		&pointLights[0],
		sizeof(PointLight));
	pixelShader->SetData(
		"pLight2",
		&pointLights[1],
		sizeof(PointLight));

	pixelShader->SetFloat4("ambientLight", XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
	pixelShader->SetFloat3("cameraPosition", player->GetTransform()->GetPosition()); //Set the player position in the pixel shader for point light specular calculations

	pixelShader->SetShaderResourceView("diffuseTexture", goMaterial->GetSRV()); //Texture
	pixelShader->SetShaderResourceView("normalMap", goMaterial->GetNormalSRV()); //Normal map, not every object has one right now, but they all probably should
	pixelShader->SetSamplerState("basicSampler", goMaterial->GetSampler()); //Sampler
	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();

	context->DrawIndexed(goMesh->GetIndexCount(), 0, 0);
}

void Renderer::Render(Emitter * emitter)
{
	// Particle states
	float blend[4] = { 1,1,1,1 };
	context->OMSetBlendState(particleBlendState, blend, 0xffffffff);  // Additive blending
	context->OMSetDepthStencilState(particleDepthState, 0);			// No depth WRITING

																	// Draw the emitter
	AssetManager::getInstance().GetPShader("ParticleShader")->SetSamplerState("trilinear",AssetManager::getInstance().GetSampler("ParticleSampler"));
	emitter->Render(context, viewMatrix, projectionMatrix);

	// Reset to default states for next frame
	context->OMSetBlendState(0, blend, 0xffffffff);
	context->OMSetDepthStencilState(0, 0);
}

ID3D11DeviceContext * Renderer::GetRenderContext()
{
	return context;
}
