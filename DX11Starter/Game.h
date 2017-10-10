#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include "GameObject.h"
#include "Lights.h"
#include "Camera.h"
#include "WICTextureLoader.h"
#include "AssetManager.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateGameObjects();//Initializes GameObjects
	void CreateMeshes();//Generates and assigns meshes for GameObjects
	void CreateMaterials();


	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	//ASSET MANAGER
	AssetManager assetManager;

	//GameObjects in scene
	GameObject sphere1;
	GameObject sphere2;
	GameObject torus1;
	GameObject torus2;
	GameObject cube1;
	GameObject cube2;

	GameObject* gameObjects[6];//Array of pointers to GameObjects so we can draw them in an easy loop	

	//Directional light
	DirectionalLight light1;
	DirectionalLight light2;

	Camera camera;
	bool freeLookEnabled = false;	//tells whether freelook is enabled 

};
