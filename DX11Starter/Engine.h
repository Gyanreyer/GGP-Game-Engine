#pragma once

#include <DirectXMath.h>
#include <vector>
#include <string> //Int conversion to string
#include "DXCore.h"
#include "SimpleShader.h"
#include "Camera.h"
#include "AssetManager.h"
#include "GameManager.h"
#include "Emitter.h"

using std::to_string; //Alternative to atoi

class Engine 
	: public DXCore
{

public:
	Engine(HINSTANCE hInstance);
	~Engine();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp (WPARAM buttonState, int x, int y);
	void OnMouseMove (int x, int y);
	void OnMouseWheel (float wheelDelta,   int x, int y);

private:
	//Screen coordinates
	RECT screen;

	//The real coordinates of the mouse
	//POINT realMouse;

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateMeshes(); //Generates and assigns meshes for GameObjects
	void CreateMaterials();
	
	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	//ASSET MANAGER
	AssetManager* assetManager;
	GameManager* gameManager;
	Renderer* renderer;


	//Skybox DX Render States 
	ID3D11RasterizerState* skyBoxRasterState;
	ID3D11DepthStencilState* skyboxDepthStencilState;

	//Post processing variables
	ID3D11RenderTargetView* ppRTV; //Allows us to render a texture
	ID3D11ShaderResourceView* bloomSRV; //Allows us to sample from that same texture
	ID3D11ShaderResourceView* ppSRV; //Allows us to sample from that same texture

	//imgui Variables
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	bool freeMouse;
};

