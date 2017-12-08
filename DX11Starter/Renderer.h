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
	Renderer(DirectX::XMFLOAT4X4 viewMat, DirectX::XMFLOAT4X4 projectMat, ID3D11DeviceContext* context, ID3D11Device* device, Player* play);
	~Renderer();
	void CreateLights();
	void SetViewProjMatrix(DirectX::XMFLOAT4X4 viewMat, DirectX::XMFLOAT4X4 projectMat);
	void Render(GameObject* gameObject);
	void RenderInstanced(vector<GameObject *> instancedGameObjects); //Actual instanced rendering
	void Render(Emitter* emitter);
	ID3D11DeviceContext* GetRenderContext();

private:
	Player* player;

	//Rendering Resources
	ID3D11DeviceContext* context;
	ID3D11Device* device;

	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

	vector<DirectionalLight> directionalLights;
	vector<PointLight> pointLights;

	//Particle Stuff
	ID3D11DepthStencilState* particleDepthState;
	ID3D11BlendState* particleBlendState;

	//Instancing variables
	ID3D11Buffer* instanceWorldMatrixBuffer;
	const byte MAXBYTEWIDTH = 27; //The maximum number of instances being drawn in one call
};