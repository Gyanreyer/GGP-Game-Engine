#include "Game.h"
#include "Vertex.h"

// For the DirectX Math library
using namespace DirectX;

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
	// Initialize fields
	vertexShader = 0;
	pixelShader = 0;

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
	delete vertexShader;
	delete pixelShader;

	//Delete meshes
	delete sphereMesh;
	delete torusMesh;
	delete cubeMesh;

	//Delete materials
	delete defaultMaterial;

	if(pebbleTextureSRV) pebbleTextureSRV->Release();
	if(sampler) sampler->Release();
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	//Create camera object
	camera = Camera(width, height);

	//Create directional lights
	light1 = { XMFLOAT4(0.1f,0.1f,0.1f,1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f),XMFLOAT3(1.0f,-1.0f,0.5f) };
	light2 = { XMFLOAT4(0,0,0,0),XMFLOAT4(1.0f,0.0f,0.0f,1.0f),XMFLOAT3(-0.5f,-0.5f,0.25f) };

	//Default prevMousePos to center of screen
	prevMousePos.x = width / 2;
	prevMousePos.y = height / 2;

	LoadShaders();
	LoadTextures();
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
	vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(L"VertexShader.cso");

	pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(L"PixelShader.cso");
}


// --------------------------------------------------------
// Creates the GameObjects we will draw in the scene and
// stores references to them in an array
// --------------------------------------------------------
void Game::CreateGameObjects()
{
	//Create 2 circle GOs
	sphere1 = GameObject(sphereMesh, defaultMaterial, context);
	sphere1.GetTransform()->SetScale(1.0f);

	sphere2 = GameObject(sphereMesh, defaultMaterial, context);
	sphere2.GetTransform()->SetScale(0.25f);

	sphere2.GetTransform()->SetPosition(0.75f, 0, 0);
	sphere2.GetTransform()->SetParent(sphere1.GetTransform());

	//Create 2 square GOs
	cube1 = GameObject(cubeMesh, defaultMaterial, context);
	cube1.GetTransform()->SetScale(0.5f);
	cube1.GetTransform()->SetPosition(0.0f,0.0f, 1.0f);

	cube1.GetTransform()->SetParent(camera.GetTransform());

	cube2 = GameObject(cubeMesh, defaultMaterial, context);
	cube2.GetTransform()->SetScale(0.5f);
	cube2.GetTransform()->SetPosition(3.0f, -1.5f, 0.0f);

	//Create 2 pentagon GOs
	torus1 = GameObject(torusMesh, defaultMaterial, context);
	torus1.GetTransform()->SetScale(0.5f);
	torus1.GetTransform()->SetPosition(-3.0f, 1.5f, 0.0f);

	torus2 = GameObject(torusMesh, defaultMaterial, context);
	torus2.GetTransform()->SetScale(0.5f);
	torus2.GetTransform()->SetPosition(-3.0f, -1.5f, 0.0f);

	//Store references to all GOs in array
	gameObjects[0] = &sphere1;
	gameObjects[1] = &sphere2;
	gameObjects[2] = &torus1;
	gameObjects[3] = &torus2;
	gameObjects[4] = &cube1;
	gameObjects[5] = &cube2;
}

// ---------------------------------------------------------
// Create meshes that GameObjects will use
// ---------------------------------------------------------
void Game::CreateMeshes()
{
	//Create meshes
	sphereMesh = new Mesh("Assets/Models/sphere.obj", device);
	cubeMesh = new Mesh("Assets/Models/cube.obj", device);
	torusMesh = new Mesh("Assets/Models/torus.obj", device);
}

void Game::LoadTextures()
{
	CreateWICTextureFromFile(device, context, L"Assets/Textures/pebbles.png", 0, &pebbleTextureSRV);

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDesc, &sampler);

	defaultMaterial = new Material(vertexShader, pixelShader, pebbleTextureSRV, sampler);//Create material with texture
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
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	float speed = 2.0f * deltaTime;//Movement speed

								   //How much to move by in each direction this frame
	float forwardSpeed = 0.0f;
	float sideSpeed = 0.0f;
	float upSpeed = 0.0f;

	if (GetAsyncKeyState('W') & 0x8000) {
		forwardSpeed += speed;//If player pressed W to go forward, add to forward speed
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		forwardSpeed -= speed;//If player pressed S to go backwards, subtract from forward speed
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		sideSpeed += speed;//If player pressed A to go left, add to side speed
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		sideSpeed -= speed;//If player pressed D to go right, subtract from side speed
	}
	if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) {
		upSpeed += speed;//If player pressed left shift to go up, add to up speed
	}
	if (GetAsyncKeyState(VK_LCONTROL) & 0x8000) {
		upSpeed -= speed;//If player pressed left control to go down, subtract from up speed
	}

	camera.GetTransform()->MoveRelative(forwardSpeed, sideSpeed, upSpeed);//Move the camera with directional speeds from input

	//Get sin and cos of current time for manipulating position and scale of objects
	float sinTime = XMScalarSin(totalTime);
	float cosTime = XMScalarCos(totalTime);

	//Rotate circle1 and make it float up and down
	sphere1.GetTransform()->Rotate(0.0f, 0.0f, 2*deltaTime);
	sphere1.GetTransform()->SetPosition(0.0f, sinTime*0.5f, 0.0f);

	//Make pentagons shrink/grow and rotate in opposite directions
	torus1.GetTransform()->SetScale((2.f + cosTime)*.1f);
	torus1.GetTransform()->Rotate(0, 0, -deltaTime*.5f);

	torus2.GetTransform()->SetScale((2.f + cosTime)*.1f);
	torus2.GetTransform()->Rotate(0, 0, deltaTime*.5f);

	//Make squares shrink/grow and rotate in opposite directions
	cube1.GetTransform()->SetScale((2.f + cosTime)*.1f);
	cube1.GetTransform()->Rotate(0, 0, deltaTime*.5f);

	cube2.GetTransform()->SetScale((2.f + cosTime)*.1f);
	cube2.GetTransform()->Rotate(0, 0, -deltaTime*.5f);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

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

	pixelShader->SetData(
		"light1",
		&light1,
		sizeof(DirectionalLight));
	pixelShader->CopyAllBufferData();

	pixelShader->SetData(
		"light2",
		&light2,
		sizeof(DirectionalLight));
	pixelShader->CopyAllBufferData();

	//Loop through GameObjects and draw them
	for (int i = 0; i < 6; i++)
	{
		gameObjects[i]->Draw(viewMat, projMat);
	}

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
	// Add any custom code here...

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
	float sensitivity = .002f;//Mouse sensitivity - higher = more sensitive

							  //Get x and y change in mouse pos
	float xChange = (x - prevMousePos.x)*sensitivity;
	float yChange = (y - prevMousePos.y)*sensitivity;

	//Rotate camera based on mouse movement
	camera.GetTransform()->Rotate(yChange,xChange,0);

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
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