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
	void RenderShadowMap();
	void Render(GameObject* gameObject);
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