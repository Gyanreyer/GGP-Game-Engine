#include "Game.h"
#include "Vertex.h"

// For the DirectX Math library
using namespace DirectX;

#if defined(DEBUG) || defined(_DEBUG)
//ImGui temp variables
static float pos[3] = { 0.0f, 0.0f, 0.0f }; //Slider for object positions
#endif

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
	// Initialize Asset Manager
	assetManager = AssetManager();

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif

}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	
	//Releases and Shuts Down Imgui
	ImGui_ImplDX11_Shutdown();

	//asset manager cleans up assets when game is deleted
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	//Initialize ImGui
	ImGui_ImplDX11_Init(hWnd, device, context);

	//Create camera object
	camera = Camera(width, height);

	//Create directional lights
	light1 = { XMFLOAT4(0.1f,0.1f,0.1f,1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f),XMFLOAT3(1.0f,-1.0f,0.5f) };
	light2 = { XMFLOAT4(0,0,0,0),XMFLOAT4(1.0f,0.0f,0.0f,1.0f),XMFLOAT3(-0.5f,-0.5f,0.25f) };

	//Default prevMousePos to center of screen
	prevMousePos.x = width / 2;
	prevMousePos.y = height / 2;

	LoadShaders();
	CreateMaterials();
	CreateMeshes();

	CreateGameObjects();


	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
	SimpleVertexShader* vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(L"VertexShader.cso");

	SimplePixelShader* pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(L"PixelShader.cso");

	//Store Vertex and Pixel Shaders into the AssetManager
	assetManager.ImportVShader("BasicVShader", vertexShader);
	assetManager.ImportPShader("BasicPShader", pixelShader);
}


// --------------------------------------------------------
// Creates the GameObjects we will draw in the scene and
// stores references to them in an array
// --------------------------------------------------------
void Game::CreateGameObjects()
{
	//Create 2 circle GOs
	sphere1 = GameObject(assetManager.GetMesh("Sphere"), assetManager.GetMaterial("StoneMat"), ColliderType::SPHERE, context);
	sphere1.GetTransform()->SetScale(1.0f);

	sphere2 = GameObject(assetManager.GetMesh("Sphere"), assetManager.GetMaterial("HazardCrateMat"), ColliderType::BOX, context);
	sphere2.GetTransform()->SetScale(0.75f);

	sphere2.GetTransform()->SetPosition(0.75f, 0, 0);

	//Create 2 square GOs
	/*cube1 = GameObject(assetManager.GetMesh("Cube"), assetManager.GetMaterial("StoneMat"), context);
	cube1.GetTransform()->SetScale(0.5f);
	cube1.GetTransform()->SetPosition(0.0f,0.0f, 1.0f);

	cube1.GetTransform()->SetParent(camera.GetTransform());*/

	cube2 = GameObject(assetManager.GetMesh("Cube"), assetManager.GetMaterial("StoneMat"), context);
	cube2.GetTransform()->SetScale(0.5f);
	cube2.GetTransform()->SetPosition(3.0f, -1.5f, 0.0f);

	//Create 2 pentagon GOs
	torus1 = GameObject(assetManager.GetMesh("Torus"), assetManager.GetMaterial("StoneMat"), context);
	torus1.GetTransform()->SetScale(0.5f);
	torus1.GetTransform()->SetPosition(-3.0f, 1.5f, 0.0f);

	torus2 = GameObject(assetManager.GetMesh("Torus"), assetManager.GetMaterial("StoneMat"), context);
	torus2.GetTransform()->SetScale(0.5f);
	torus2.GetTransform()->SetPosition(-3.0f, -1.5f, 0.0f);

	//Store references to all GOs in array
	gameObjects[0] = &sphere1;
	gameObjects[1] = &sphere2;
	gameObjects[2] = &torus1;
	gameObjects[3] = &torus2;
	//gameObjects[4] = &cube1;
	gameObjects[4] = &cube2;
}

// ---------------------------------------------------------
// Create meshes that GameObjects will use
// ---------------------------------------------------------
void Game::CreateMeshes()
{
	//Create meshes
	assetManager.ImportMesh("Cone", new Mesh("../../DX11Starter/Assets/Models/cone.obj", device));
	assetManager.ImportMesh("Cube", new Mesh("../../DX11Starter/Assets/Models/cube.obj", device));
	assetManager.ImportMesh("Cylinder", new Mesh("../../DX11Starter/Assets/Models/cylinder.obj", device));
	assetManager.ImportMesh("Helix", new Mesh("../../DX11Starter/Assets/Models/helix.obj", device));
	assetManager.ImportMesh("Sphere", new Mesh("../../DX11Starter/Assets/Models/sphere.obj", device));
	assetManager.ImportMesh("Torus", new Mesh("../../DX11Starter/Assets/Models/torus.obj", device));
	assetManager.ImportMesh("Cactus", new Mesh("../../DX11Starter/Assets/Models/cactus.obj", device));
}

///Loads in textures and makes them into materials
void Game::CreateMaterials()
{
	//create Texture
	ID3D11ShaderResourceView* hazardTexture;
	HRESULT tResult = CreateWICTextureFromFile(device, context, L"../../DX11Starter/Assets/Textures/HazardCrateTexture.jpg", 0, &hazardTexture);
	if (tResult != S_OK) {
		printf("Hazard Texture is could not be loaded");
	}
	assetManager.ImportTexture("HazardTexture", hazardTexture);

	//Create Sampler State
	ID3D11SamplerState* sample;
	D3D11_SAMPLER_DESC sampleDesc = {};
	//Describes how to handle addresses outside 0-1 UV range
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	//Describes how to handle sampling between pixels
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT sampleResult = device->CreateSamplerState(&sampleDesc, &sample);
	if (sampleResult != S_OK) {
		printf("Sample State could not be created");
	}

	assetManager.ImportSampler("BasicSampler", sample);

	//Create Material 
	Material* genericMat = new Material(assetManager.GetVShader("BasicVShader"), assetManager.GetPShader("BasicPShader"), assetManager.GetTexture("HazardTexture"), assetManager.GetSampler("BasicSampler"));
	assetManager.ImportMaterial("HazardCrateMat", genericMat);

	//create Texture
	ID3D11ShaderResourceView* stoneTexture;
	tResult = CreateWICTextureFromFile(device, context, L"../../DX11Starter/Assets/Textures/GreyStoneTexture.jpg", 0, &stoneTexture);
	if (tResult != S_OK) {
		printf("Stone Texture is could not be loaded");
	}

	assetManager.ImportTexture("Stone", stoneTexture);

	Material* stoneMat = new Material(assetManager.GetVShader("BasicVShader"), assetManager.GetPShader("BasicPShader"), assetManager.GetTexture("Stone"), assetManager.GetSampler("BasicSampler"));
	assetManager.ImportMaterial("StoneMat", stoneMat);
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	camera.UpdateProjectionMatrix(width, height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;

	//All Imgui UI stuff must be done between imgui newFrame() and imgui render()
	ImGui_ImplDX11_NewFrame();

	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	camera.Update(deltaTime);

	//Get sin and cos of current time for manipulating position and scale of objects
	float sinTime = XMScalarSin(totalTime);
	float cosTime = XMScalarCos(totalTime);

	//Rotate circle1 and make it float up and down
	sphere1.GetTransform()->Rotate(0.0f, 0.0f, 2*deltaTime);
	//sphere1.GetTransform()->SetPosition(0.0f, sinTime*0.5f, 0.0f);

	sphere2.GetTransform()->SetPosition(pos[0], pos[1], pos[2]);
	Collision::CheckCollisionSphereBox(sphere1.GetCollider(), sphere2.GetCollider());

	//Make pentagons shrink/grow and rotate in opposite directions
	torus1.GetTransform()->SetScale((2.f + cosTime)*.1f);
	torus1.GetTransform()->Rotate(0, 0, -deltaTime*.5f);

	torus2.GetTransform()->SetScale((2.f + cosTime)*.1f);
	torus2.GetTransform()->Rotate(0, 0, deltaTime*.5f);

	//Make squares shrink/grow and rotate in opposite directions
	/*cube1.GetTransform()->SetScale((2.f + cosTime)*.1f);
	cube1.GetTransform()->Rotate(0, 0, deltaTime*.5f);*/

	cube2.GetTransform()->SetScale((2.f + cosTime)*.1f);
	cube2.GetTransform()->Rotate(0, 0, -deltaTime*.5f);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { clear_color.x, clear_color.y, clear_color.z, clear_color.w };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	XMFLOAT4X4 viewMat = camera.GetViewMatrix();
	XMFLOAT4X4 projMat = camera.GetProjectionMatrix();

	//Loop through GameObjects and draw them
	for (int i = 0; i < 5; i++)
	{
		gameObjects[i]->GetMaterial()->GetPixelShader()->SetData(
			"light1",
			&light1,
			sizeof(DirectionalLight));

		gameObjects[i]->GetMaterial()->GetPixelShader()->SetData(
			"light2",
			&light2,
			sizeof(DirectionalLight));

		//gameObjects[i]->GetMaterial()->GetPixelShader()->CopyAllBufferData(); //This is done through the draw call
		gameObjects[i]->Draw(viewMat, projMat);
	}

	 
	// 1. Show a simple window
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
	{
		static float f = 0.0f;
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::SliderFloat3("Sphere2 Pos", pos, -5, 5);
		ImGui::ColorEdit3("clear color", (float*)&clear_color);
		ImGui::Checkbox("Free Look Enabled", &freelookEnabled);
		//if (ImGui::Button("Test Window")) show_test_window ^= 1;
		//if (ImGui::Button("Another Window")) show_another_window ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	ImGui::Render();
	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	//io.MouseDown[0] = true;
	//io.MouseDown[1] = true;

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	//io.MouseDown[0] = false;
	//io.MouseDown[1] = false;

	// Add any custom code here...

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x,(float)y);

	//float sensitivity = .002f;//Mouse sensitivity - higher = more sensitive
	//if (freeLookEnabled) 
	//{
	//	//Get x and y change in mouse pos
	//	float xChange = (x - prevMousePos.x)*sensitivity;
	//	float yChange = (y - prevMousePos.y)*sensitivity;
	//
	//	//Rotate camera based on mouse movement
	//	camera.GetTransform()->Rotate(yChange, xChange, 0);
	//}
	//
	//// Save the previous mouse position, so we have it for the future
	//prevMousePos.x = x;
	//prevMousePos.y = y;

	//When the left mouse button is held down and freelook is enabled
	if (buttonState && 0x0001 && freelookEnabled)
	{
		//Move the camera with the mouse
		float nextX = x - (float)prevMousePos.x;
		float nextY = y - (float)prevMousePos.y;

		camera.MouseInput(nextX, nextY);

		// Save the previous mouse position, so we have it for the future
		prevMousePos.x = x;
		prevMousePos.y = y;
	}
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion