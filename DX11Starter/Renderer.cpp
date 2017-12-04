#include "Renderer.h"

Renderer::Renderer(DirectX::XMFLOAT4X4 viewMat, DirectX::XMFLOAT4X4 projectMat, ID3D11DeviceContext * context, ID3D11Device * device, int w, int h, Player* play)
{
	player = play;

	width = w; 
	height = h;

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

	//Create Shadow Map Setup
	shadowMapSize = 2048;

	//Create the actual texture that will be the shadow map
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	shadowDesc.Width = shadowMapSize;
	shadowDesc.Height = shadowMapSize;
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	ID3D11Texture2D* shadowTexture;
	HRESULT hr = device->CreateTexture2D(&shadowDesc, 0, &shadowTexture);

	//Create the depth/stencil
	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
	shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(shadowTexture, &shadowDSDesc, &shadowDSV);

	//create the Shader Resource View for shadow map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(shadowTexture, &srvDesc, &shadowSRV);

	//Release the Texture reference since we don't need it 
	shadowTexture->Release();

	//create the special "comparison" sampler state for shadows
	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // Could be anisotropic
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f;
	shadowSampDesc.BorderColor[1] = 1.0f;
	shadowSampDesc.BorderColor[2] = 1.0f;
	shadowSampDesc.BorderColor[3] = 1.0f;
	device->CreateSamplerState(&shadowSampDesc, &shadowSampler);

	//Create Shadow Rasterizer State
	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Multiplied by (smallest possible value > 0 in depth buffer)
	shadowRastDesc.DepthBiasClamp = 0.0f;
	shadowRastDesc.SlopeScaledDepthBias = 1.0f;
	device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizer);

	shadowVS = AssetManager::getInstance().GetVShader("ShadowShader");
}

Renderer::~Renderer()
{
	particleBlendState->Release();
	particleDepthState->Release();

	// Clean up shadow map
	shadowDSV->Release();
	shadowSRV->Release();
	shadowRasterizer->Release();
	shadowSampler->Release();
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

	// We need to pass the shadow "creation" matrices in here
	// so we can reconstruct the shadow map position
	vertexShader->SetMatrix4x4("shadowView", shadowViewMatrix);
	vertexShader->SetMatrix4x4("shadowProj", shadowProjectionMatrix);

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
	pixelShader->SetShaderResourceView("ShadowMap", shadowSRV);
	pixelShader->SetSamplerState("basicSampler", goMaterial->GetSampler()); //Sampler
	pixelShader->SetSamplerState("ShadowSampler", shadowSampler);
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

void Renderer::SetSceneObjects(vector<GameObject*> objects)
{
	sceneObjects = objects;
}

void Renderer::RenderShadowMap(ID3D11RenderTargetView* oldRenderTargetView, ID3D11DepthStencilView* oldDepthStencilView)
{
	//ID3D11RenderTargetView* oldRenderTargetView;
	//ID3D11DepthStencilView* oldDepthStencilView;

	//context->OMGetRenderTargets(1, &oldRenderTargetView, &oldDepthStencilView);

	//Change which depth buffer we are rendering into
	context->OMSetRenderTargets(0, 0, shadowDSV);
	context->ClearDepthStencilView(shadowDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//Set up any required rendering states
	context->RSSetState(shadowRasterizer);

	//create a viewport that matches the render target size
	D3D11_VIEWPORT shadowViewport = {};
	shadowViewport.TopLeftX = 0;
	shadowViewport.TopLeftY = 0;
	shadowViewport.Width = (float)shadowMapSize;
	shadowViewport.Height = (float)shadowMapSize;
	shadowViewport.MinDepth = 0.0f;
	shadowViewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &shadowViewport);

	// Create my shadow map matrices
	XMMATRIX shadowView = XMMatrixLookAtLH(
		XMVectorSet(0, 20, -20, 0), // Eye position
		XMVectorSet(0, 0, 0, 0),	// Looking at (0,0,0)
		XMVectorSet(0, 1, 0, 0));	// Up (0,1,0)

	XMStoreFloat4x4(&shadowViewMatrix, XMMatrixTranspose(shadowView));

	XMMATRIX shadowProj = XMMatrixOrthographicLH(
		10.0f,		// Width of the projection in world units
		10.0f,		// Height of the projection in world units
		0.1f,		// Near clip
		100.0f);	// Far clip

	XMStoreFloat4x4(&shadowProjectionMatrix, XMMatrixTranspose(shadowProj));

	//Turn on our shadow map shaders
	shadowVS->SetShader();
	shadowVS->SetMatrix4x4("view", shadowViewMatrix);
	shadowVS->SetMatrix4x4("projection", shadowProjectionMatrix);

	//turn off the pixel shader because we don't need it for this stage
	context->PSSetShader(0, 0, 0);

	//Render all of the entities in the scene
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	for (unsigned int i = 0; i < sceneObjects.size(); i++)
	{
		//Grab GameObject from vector objects
		GameObject* object = sceneObjects[i];
		ID3D11Buffer* vertexBuff = object->GetMesh()->GetVertexBuffer();
		ID3D11Buffer* indexBuff = object->GetMesh()->GetIndexBuffer();

		//Set buffers in the input assembler
		context->IASetVertexBuffers(0, 1, &vertexBuff, &stride, &offset);
		context->IASetIndexBuffer(indexBuff, DXGI_FORMAT_R32_UINT, 0);

		//Use the Shadow Vertex Shader
		shadowVS->SetMatrix4x4("world", object->GetWorldMatrix());
		shadowVS->CopyAllBufferData();

		//Do Shadow Drawing
		context->DrawIndexed(object->GetMesh()->GetIndexCount(), 0, 0);
	}

	//Restore states and render options  that were changed for shadow rendering
	context->OMSetRenderTargets(1, &oldRenderTargetView, oldDepthStencilView);
	context->RSSetState(0);

	shadowViewport.Width = width;
	shadowViewport.Height = height;
	context->RSSetViewports(1, &shadowViewport);

	////Release old Render states
	//oldDepthStencilView->Release();
	//oldRenderTargetView->Release();
}
