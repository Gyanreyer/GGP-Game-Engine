#pragma once

#include <DirectXMath.h>
#include <vector>
#include "DXCore.h"
#include "SimpleShader.h"
//#include "GameObject.h"
//#include "Collider.h"	 //Game Manager + Others
//#include "Collision.h" //Game Manager + Others
#include "Camera.h"
#include "AssetManager.h"
//#include "Player.h" //GameManager
//#include "Enemy.h"	//GameManager
//#include "ProjectileManager.h"	//GameManager
#include "GameManager.h"

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
	void OnMouseMove (WPARAM buttonState, int x, int y);
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

	//Directional light
	//DirectionalLight light1;
	//DirectionalLight light2;

	//Figure out implementing free cam later
	//Camera camera;
	//bool freelookEnabled;

	//The player and enemy
	//Player* player;
	//ProjectileManager* projectileManager;

	//Don't need this due to some thing I did but forgot how it works
	//bool freeLookEnabled = false;	//tells whether freelook is enabled 

	//imgui Variables
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};

