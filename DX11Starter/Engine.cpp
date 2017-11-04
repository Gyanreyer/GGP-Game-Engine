#include "Engine.h"
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
Engine::Engine(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Engine",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
	// Initialize Asset Manager
	assetManager = &AssetManager::getInstance();

	//Set cursor position to center of window
	//https://stackoverflow.com/questions/8690619/how-to-get-screen-resolution-in-c
	//Hide cursor
	GetWindowRect(GetDesktopWindow(), &screen); //Get the dimensions of the desktop
	SetCursorPos(screen.right / 2, screen.bottom / 2);
	//ShowCursor(false);

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
Engine::~Engine()
{
	//Releases and Shuts Down Imgui
	ImGui_ImplDX11_Shutdown();

	//asset manager cleans up assets when game is deleted
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Engine::Init()
{
	//Initialize ImGui
	ImGui_ImplDX11_Init(hWnd, device, context);

	gameManager = &GameManager::getInstance();

	//Create camera object
	//camera = Camera(width, height);

	//Create directional lights
	/*light1 = { XMFLOAT4(0.1f,0.1f,0.1f,1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f),XMFLOAT3(1.0f,-1.0f,0.5f) };
	light2 = { XMFLOAT4(0,0,0,0),XMFLOAT4(1.0f,0.0f,0.0f,1.0f),XMFLOAT3(-0.5f,-0.5f,0.25f) };*/

	//Default prevMousePos to center of screen
	prevMousePos.x = width / 2;
	prevMousePos.y = height / 2;

	//Set the starting point for the real mouth coordinates
	//realMouse.x = screen.right / 2;
	//realMouse.y = screen.bottom / 2;

	LoadShaders();
	CreateMaterials();
	CreateMeshes();

	//gameManager->StartGame(&assetManager, (float)width, (float)height, context); //starts the game
	//player = gameManager->GetPlayer(); //give engine a refrence to player

	//Set up projectile manager
	//projectileManager = gameManager->GetProjectileManager();

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
void Engine::LoadShaders()
{
	SimpleVertexShader* vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(L"VertexShader.cso");

	SimplePixelShader* pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(L"PixelShader.cso");

	//Store Vertex and Pixel Shaders into the AssetManager
	assetManager->StoreVShader("BasicVShader", vertexShader);
	assetManager->StorePShader("BasicPShader", pixelShader);
}

// ---------------------------------------------------------
// Create meshes that GameObjects will use
// ---------------------------------------------------------
void Engine::CreateMeshes()
{
	//Move to asset Manager Loading
	//Create meshes
	assetManager->ImportMesh("Cone", "../../DX11Starter/Assets/Models/cone.obj", device);
	assetManager->ImportMesh("Cube", "../../DX11Starter/Assets/Models/cube.obj", device);
	assetManager->ImportMesh("Cylinder", "../../DX11Starter/Assets/Models/cylinder.obj", device);
	assetManager->ImportMesh("Helix", "../../DX11Starter/Assets/Models/helix.obj", device);
	assetManager->ImportMesh("Sphere", "../../DX11Starter/Assets/Models/sphere.obj", device);
	assetManager->ImportMesh("Torus", "../../DX11Starter/Assets/Models/torus.obj", device);
	assetManager->ImportMesh("Cactus", "../../DX11Starter/Assets/Models/cactus.obj", device);
	assetManager->ImportMesh("RustyPete", "../../DX11Starter/Assets/Models/RustyPete/RustyPete.obj", device);
	assetManager->ImportMesh("PurpleGhost", "../../DX11Starter/Assets/Models/ghost.obj", device);
	assetManager->ImportMesh("Plane", "../../DX11Starter/Assets/Models/Quad.obj", device);
}

///Loads in textures and makes them into materials
void Engine::CreateMaterials()
{
	//Asset Manager Loading
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

	assetManager->StoreSampler("BasicSampler", sample);

	//create Texture
	assetManager->ImportTexture("HazardTexture", L"../../DX11Starter/Assets/Textures/HazardCrateTexture.jpg", device, context);
	assetManager->CreateMaterial("HazardCrateMat", "BasicVShader", "BasicPShader", "HazardTexture", "BasicSampler");
	assetManager->ImportTexture("RustyPete", L"../../DX11Starter/Assets/Models/RustyPete/rusty_pete_body_c.png", device, context);
	assetManager->CreateMaterial("RustyPeteMaterial", "BasicVShader", "BasicPShader", "RustyPete", "BasicSampler");
	assetManager->ImportTexture("Stone", L"../../DX11Starter/Assets/Textures/GreyStoneTexture.jpg", device, context);
	assetManager->CreateMaterial("StoneMat", "BasicVShader", "BasicPShader", "Stone", "BasicSampler");
	assetManager->ImportTexture("EnemyTexture", L"../../DX11Starter/Assets/Textures/aaaaaa.png", device, context);
	assetManager->CreateMaterial("EnemyMaterial", "BasicVShader", "BasicPShader", "EnemyTexture", "BasicSampler");
	assetManager->ImportTexture("PurpleGhost", L"../../DX11Starter/Assets/Textures/ghost-dark.png", device, context);
	assetManager->CreateMaterial("PurpleGhost", "BasicVShader", "BasicPShader", "PurpleGhost", "BasicSampler");
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Engine::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	//player->UpdateProjectionMatrix(width,height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Engine::Update(float deltaTime, float totalTime)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;

	//All Imgui UI stuff must be done between imgui newFrame() and imgui render()
	ImGui_ImplDX11_NewFrame();

	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	//////Engine update Loop
	gameManager->GameUpdate(deltaTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Engine::Draw(float deltaTime, float totalTime)
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

	// 1. Show a simple window
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
	{
		static float f = 0.0f;
		static char testText = char();
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", (float*)&clear_color);
		ImGui::InputText("Text Test", &testText, sizeof(char) * 50);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	//XMFLOAT4X4 viewMat = player->GetViewMatrix();
	//XMFLOAT4X4 projMat = player->GetProjectionMatrix();

	////Get all the GameObjects now
	////The GameObject vector doesn't change, so this should be optimal
	//vector<GameObject>* goVector = gameManager->GetGameObjectVector();

	////Get the vector of enemies
	//vector<Enemy>* enemies = gameManager->GetEnemyVector();

	////Loop through GameObjects and draw them
	//for (byte i = 0; i < goVector->size(); i++)
	//{
	//	(*goVector)[i].GetMaterial()->GetPixelShader()->SetData(
	//		"light1",
	//		&light1,
	//		sizeof(DirectionalLight));

	//	(*goVector)[i].GetMaterial()->GetPixelShader()->SetData(
	//		"light2",
	//		&light2,
	//		sizeof(DirectionalLight));

	//	(*goVector)[i].Draw(viewMat, projMat);
	//}

	////Loop through Enemies and draw them
	//for (byte i = 0; i < enemies->size(); i++)
	//{
	//	(*enemies)[i].GetMaterial()->GetPixelShader()->SetData(
	//		"light1",
	//		&light1,
	//		sizeof(DirectionalLight));

	//	(*enemies)[i].GetMaterial()->GetPixelShader()->SetData(
	//		"light2",
	//		&light2,
	//		sizeof(DirectionalLight));

	//	(*enemies)[i].Draw(viewMat, projMat);
	//}

	////Set up light data for projectile materials
	//projectileManager->SetProjectileShaderData("light1", &light1, sizeof(DirectionalLight));
	//projectileManager->SetProjectileShaderData("light2", &light2, sizeof(DirectionalLight));

	////Draw all projectiles
	//projectileManager->DrawProjectiles(viewMat, projMat);


	////Display game stats
	//std::string score = "Score: ";
	//char intChar[10];
	//score += itoa(gameManager->GetGameScore() ,intChar, 10);
	//std::string health = "Health: ";
	//health += itoa(player->GetHealth(), intChar, 10);
	//std::string timeLeft = "Time Left: ";
	//timeLeft += itoa((int)gameManager->getTimeLeft(), intChar, 10);
	//ImGui::Begin("GGP Game", (bool*)1);
	//ImGui::Text(timeLeft.c_str());
	//ImGui::Text(health.c_str());
	//ImGui::Text(score.c_str());
	//ImGui::End();

	//
	//if (ImGui::BeginPopup("EndGame")) {
	//	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Game is over");
	//	std::string finalScore = "Final Score: ";
	//	finalScore += itoa(gameManager->GetGameScore(), intChar, 10);
	//	ImGui::Text(finalScore.c_str());
	//	if (ImGui::Button("Restart Game"))
	//	{
	//		gameManager->StartGame(&assetManager, (float)(width), (float)(height), context);
	//		ImGui::CloseCurrentPopup();
	//	}
	//	ImGui::EndPopup();
	//}
	gameManager->GameDraw();
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
void Engine::OnMouseDown(WPARAM buttonState, int x, int y)
{

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Capture the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);

	//When the left mouse button is pressed
	//Duh, it's a bitwise &
	if (buttonState & MK_LBUTTON)
	{
		//Make Player function to shoot
		//Transform* pt = player->GetTransform();

		////Make player shoot
		//projectileManager->SpawnPlayerProjectile(pt->GetPosition(), pt->GetForward());
	}
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Engine::OnMouseUp(WPARAM buttonState, int x, int y)
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
void Engine::OnMouseMove(WPARAM buttonState, int x, int y)
{

	//When the right mouse button is pressed
	//Duh, it's a bitwise &
	if (buttonState & MK_RBUTTON)
	{
		//Distance the mouse moves in one frame
		float deltaX = x - (float)prevMousePos.x;
		float deltaY = y - (float)prevMousePos.y;

		//Rotate player
		//player->UpdateMouseInput(deltaX, deltaY);

		//Update previous mose position
		prevMousePos.x = x;
		prevMousePos.y = y;

	}
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Engine::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion