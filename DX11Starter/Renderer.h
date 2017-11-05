#pragma once
#include <d3d11.h>
#include "GameObject.h"
#include "Lights.h"

class Renderer
{
public:
	Renderer(DirectX::XMFLOAT4X4 viewMat, DirectX::XMFLOAT4X4 projectMat, ID3D11DeviceContext* context,ID3D11Device* device);
	~Renderer();
	void CreateLights();
	void SetViewProjMatrix(DirectX::XMFLOAT4X4 viewMat, DirectX::XMFLOAT4X4 projectMat);
	void Render(GameObject* gameObject);

private:
	//Rendering Resources
	ID3D11DeviceContext* context;
	ID3D11Device* device;

	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

	vector<DirectionalLight> directionalLights;
};

