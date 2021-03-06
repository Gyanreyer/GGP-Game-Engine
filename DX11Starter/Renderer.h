#pragma once
#include <d3d11.h>
#include "GameObject.h"
#include "Lights.h"
#include "Player.h"
#include "Emitter.h"
#include "AssetManager.h"

class Player;
class Renderer
{
public:
	Renderer(DirectX::XMFLOAT4X4 viewMat, DirectX::XMFLOAT4X4 projectMat, ID3D11DeviceContext* context, ID3D11Device* device, int w, int h, Player* play);
	~Renderer();
	void CreateLights();
	void SetViewProjMatrix(DirectX::XMFLOAT4X4 viewMat, DirectX::XMFLOAT4X4 projectMat);
	void RenderShadowMap(ID3D11RenderTargetView* oldRenderTargetView, ID3D11DepthStencilView* oldDepthStencilView);
	void Render(GameObject* gameObject);
	void RenderInstanced(vector<GameObject *> instancedGameObjects); //Actual instanced rendering
	void Render(Emitter* emitter);
	ID3D11DeviceContext* GetRenderContext();
	void SetSceneObjects(vector<GameObject*> objects);

private:
	Player* player;
	vector<GameObject*> sceneObjects;

	//Rendering Resources
	ID3D11DeviceContext* context;
	ID3D11Device* device;
	int width;
	int height;

	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

	vector<DirectionalLight> directionalLights;
	vector<PointLight> pointLights;

	//Particle Stuff
	ID3D11DepthStencilState* particleDepthState;
	ID3D11BlendState* particleBlendState;

	//Base shaders
	SimpleVertexShader* baseVS; //Everything uses the same vertex shader in rendering, so don't waste time pulling it from every object
	SimplePixelShader* basePS;

	//Basic sampler
	ID3D11SamplerState* basicSampler; //Every GameObject uses the same sampler when drawing, so just get it once

	//Previous pixel shader, for optimization
	SimplePixelShader* prevPixelShader;

	//Instancing variables
	ID3D11Buffer* instanceWorldMatrixBuffer;
	const byte MAXBYTEWIDTH = 27; //The maximum number of instances being drawn in one call

	//Shadow information
	int shadowMapSize; //should be a power of 2
	ID3D11DepthStencilView* shadowDSV;
	ID3D11ShaderResourceView* shadowSRV;
	ID3D11SamplerState* shadowSampler; //move to asset manager
	ID3D11RasterizerState* shadowRasterizer;
	SimpleVertexShader* shadowVS; //holds a refrence to shadow vertex shader given by asset manager for convience
	DirectX::XMFLOAT4X4 shadowViewMatrix;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix;
};