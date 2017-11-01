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
Game::~Game()
{
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

	gameManager = &GameManager::getInstance();

	//Create camera object
	//camera = Camera(width, height);


	//Create directional lights
	light1 = { XMFLOAT4(0.1f,0.1f,0.1f,1.0f),XMFLOAT4(1.0f,1.0f,1.0f,1.0f),XMFLOAT3(1.0f,-1.0f,0.5f) };
	light2 = { XMFLOAT4(0,0,0,0),XMFLOAT4(1.0f,0.0f,0.0f,1.0f),XMFLOAT3(-0.5f,-0.5f,0.25f) };

	//Default prevMousePos to center of screen
	prevMousePos.x = width / 2;
	prevMousePos.y = height / 2;

	//Set the starting point for the real mouth coordinates
	//realMouse.x = screen.right / 2;
	//realMouse.y = screen.bottom / 2;

	LoadShaders();
	CreateMaterials();
	CreateMeshes();

	CreateGameObjects();

	gameManager->StartGame(&assetManager, width, height, context); //starts the game
	player = gameManager->GetPlayer(); //give engine a refrence to player

	//Set up projectile manager
	projectileManager = gameManager->GetProjectileManager();

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
	assetManager.ImportMesh("RustyPete", new Mesh("../../DX11Starter/Assets/Models/RustyPete/RustyPete.obj", device));
	assetManager.ImportMesh("PurpleGhost", new Mesh("../../DX11Starter/Assets/Models/ghost.obj", device));
	assetManager.ImportMesh("Plane", new Mesh("../../DX11Starter/Assets/Models/Quad.obj", device));
}

///Loads in textures and makes them into materials
void Game::CreateMaterials()
{
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

	//create Texture
	ID3D11ShaderResourceView* hazardTexture;
	HRESULT tResult = CreateWICTextureFromFile(device, context, L"../../DX11Starter/Assets/Textures/HazardCrateTexture.jpg", 0, &hazardTexture);
	if (tResult != S_OK) {
		printf("Hazard Texture is could not be loaded");
	}
	assetManager.ImportTexture("HazardTexture", hazardTexture);

	ID3D11ShaderResourceView* rustyPeteTexture;
	tResult = CreateWICTextureFromFile(device, context, L"../../DX11Starter/Assets/Models/RustyPete/rusty_pete_body_c.png", 0, &rustyPeteTexture);
	assetManager.ImportTexture("RustyPete", rustyPeteTexture);

	Material* rustyPeteMaterial = new Material(assetManager.GetVShader("BasicVShader"), assetManager.GetPShader("BasicPShader"), assetManager.GetTexture("RustyPete"), assetManager.GetSampler("BasicSampler"));
	assetManager.ImportMaterial("RustyPeteMaterial", rustyPeteMaterial);

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

	//Create enemy texture and material
	ID3D11ShaderResourceView* enemyTexture;
	CreateWICTextureFromFile(device, context, L"../../DX11Starter/Assets/Textures/aaaaaa.png", 0, &enemyTexture);
	assetManager.ImportTexture("EnemyTexture", enemyTexture);
	Material* enemyMaterial = new Material(assetManager.GetVShader("BasicVShader"), assetManager.GetPShader("BasicPShader"), assetManager.GetTexture("EnemyTexture"), assetManager.GetSampler("BasicSampler"));
	assetManager.ImportMaterial("EnemyMaterial", enemyMaterial);

	ID3D11ShaderResourceView* purpleGhostTexture;
	CreateWICTextureFromFile(device, context, L"../../DX11Starter/Assets/Textures/ghost-dark.png", 0, &purpleGhostTexture);
	assetManager.ImportTexture("PurpleGhost", purpleGhostTexture);
	Material* purpleGhostMat = new Material(assetManager.GetVShader("BasicVShader"), assetManager.GetPShader("BasicPShader"), assetManager.GetTexture("PurpleGhost"), assetManager.GetSampler("BasicSampler"));
	assetManager.ImportMaterial("PurpleGhost", purpleGhostMat);
}

// --------------------------------------------------------
// Creates the GameObjects we will draw in the scene and
// stores references to them in an array
// --------------------------------------------------------
void Game::CreateGameObjects()
{
	///OTHER GAMEOBJECTS
	floor = GameObject(assetManager.GetMesh("Plane"), assetManager.GetMaterial("RustyPeteMaterial"), BOX, false, context);
	floor.GetTransform()->SetScale(10, 0.001f, 10); //The floor is real small, for the sake of collisions

	obstacle = GameObject(assetManager.GetMesh("Cube"), assetManager.GetMaterial("StoneMat"), BOX, false, context);
	obstacle.GetTransform()->SetPosition(2, 0.5f, -2); //The floor is real small, for the sake of collisions

	//Store references to all GOs in array
	gameObjects.push_back(&floor);
	gameObjects.push_back(&obstacle);
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	player->UpdateProjectionMatrix(width,height);
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

	////Game update Loop
	//1. Make sure game is has not ended
	if (!gameManager->isGameOver()) {

		player->Update(deltaTime);

		projectileManager->UpdateProjectiles(deltaTime);
		vector<Enemy>* enemies = gameManager->GetEnemyVector(); //get enemies

		for (int i = 0; i < enemies->size(); i++) {
			(*enemies)[i].Update(deltaTime);
		}

		//PLAYER PROJECTILE COLLISIONS
		for (byte i = 0; i < projectileManager->GetPlayerProjectiles().size(); i++)
		{
			Collider projCollider = *projectileManager->GetPlayerProjectiles()[i].GetCollider(); //The projectile's collider

			//WITH ENEMIES
			for (byte j = 0; j < enemies->size(); j++)
			{
				if (Collision::CheckCollisionSphereBox(&projCollider, (*enemies)[j].GetCollider()))
				{
					//Add score to player score
					gameManager->AddScore((*enemies)[j].GetPoints());
					(*enemies).erase((*enemies).begin() + j);
					projectileManager->RemovePlayerProjectile(i);
					goto break1; //Get out of the loop to prevent vector subscript errors
				}
			}

			//WITH OTHER GAMEOBJECTS
			for (byte j = 0; j < gameObjects.size(); j++)
			{
				Collider* goCollider = gameObjects[j]->GetCollider(); //The GameObject's collider

				if (goCollider->collType == BOX && Collision::CheckCollisionSphereBox(&projCollider, goCollider))
				{
					projectileManager->RemovePlayerProjectile(i); //Simply delete projectile
					goto break1; //Get out of the loop to prevent vector subscript errors
				}
				else if (goCollider->collType == SPHERE && Collision::CheckCollisionSphereSphere(&projCollider, goCollider))
				{
					projectileManager->RemovePlayerProjectile(i); //Simply delete projectile
					goto break1; //Get out of the loop to prevent vector subscript errors
				}
			}
		}

		break1: //This is super useful and I'm sad I didn't know about it sooner
		//ENEMY PROJECTILE COLLISIONS
		for (byte i = 0; i < projectileManager->GetEnemyProjectiles().size(); i++)
		{
			Collider projCollider = *projectileManager->GetEnemyProjectiles()[i].GetCollider(); //The projectile's collider

			//WITH OTHER GAMEOBJECTS
			for (byte j = 0; j < gameObjects.size(); j++)
			{
				Collider* goCollider = gameObjects[j]->GetCollider(); //The GameObject's collider

				if (goCollider->collType == BOX && Collision::CheckCollisionSphereBox(&projCollider, goCollider))
				{
					projectileManager->RemoveEnemyProjectile(i); //Simply delete projectile
					goto break2; //Get out of the loop to prevent vector subscript errors
				}
				else if (goCollider->collType == SPHERE && Collision::CheckCollisionSphereSphere(&projCollider, goCollider))
				{
					projectileManager->RemoveEnemyProjectile(i); //Simply delete projectile
					goto break2; //Get out of the loop to prevent vector subscript errors
				}
			}

			//WITH PLAYER
			if (Collision::CheckCollisionSphereBox(&projCollider, player->GetCollider()))
			{
				player->DecrementHealth();
				projectileManager->RemoveEnemyProjectile(i); //Remove the enemy's projectile, prevents multi-frame collisions
				goto break2; //Get out of the loop to prevent vector subscript errors
			}
		}

		break2: //This is super useful and I'm sad I didn't know about it sooner
		//GAMEOBJECT COLLISIONS
		//Start at 1, 0 is the ground
		for (byte i = 1; i < gameObjects.size(); i++)
		{
			//WITH PLAYER
			Collider* goCollider = gameObjects[i]->GetCollider(); //The GameObject's collider
			Collider* pCollider = player->GetCollider(); //The player's collider

			if (goCollider->collType == BOX && Collision::CheckCollisionBoxBox(goCollider, pCollider))
			{
				//TODO: collision resolution
				printf("Here\n");
				break;
			}
			else if(goCollider->collType == SPHERE && Collision::CheckCollisionSphereBox(goCollider, pCollider))
			{
				//TODO: collision resolution
				break;
			}
		}
	}
	else {
		ImGui::OpenPopup("EndGame");
	}
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

	XMFLOAT4X4 viewMat = player->GetViewMatrix();
	XMFLOAT4X4 projMat = player->GetProjectionMatrix();

	vector<Enemy>* enemies = gameManager->GetEnemyVector(); //get enemies
	//Loop through GameObjects and draw them
	for (byte i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->GetMaterial()->GetPixelShader()->SetData(
			"light1",
			&light1,
			sizeof(DirectionalLight));

		gameObjects[i]->GetMaterial()->GetPixelShader()->SetData(
			"light2",
			&light2,
			sizeof(DirectionalLight));

		gameObjects[i]->Draw(viewMat, projMat);
	}

	//Loop through Enemies and draw them
	for (byte i = 0; i < enemies->size(); i++)
	{
		(*enemies)[i].GetMaterial()->GetPixelShader()->SetData(
			"light1",
			&light1,
			sizeof(DirectionalLight));

		(*enemies)[i].GetMaterial()->GetPixelShader()->SetData(
			"light2",
			&light2,
			sizeof(DirectionalLight));

		(*enemies)[i].Draw(viewMat, projMat);
	}

	//Set up light data for projectile materials
	projectileManager->SetProjectileShaderData("light1", &light1, sizeof(DirectionalLight));
	projectileManager->SetProjectileShaderData("light2", &light2, sizeof(DirectionalLight));

	//Draw all projectiles
	projectileManager->DrawProjectiles(viewMat, projMat);

	// 1. Show a simple window
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
	{
		static float f = 0.0f;
		static char testText = char();
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::SliderFloat3("Sphere2 Pos", pos, -5, 5);
		ImGui::ColorEdit3("clear color", (float*)&clear_color);
		ImGui::InputText("Text Test", &testText, sizeof(char)* 50);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	//Display game stats
	std::string score = "Score: ";
	char intChar[10];
	score += itoa(gameManager->GetGameScore() ,intChar, 10);
	std::string health = "Health: ";
	health += itoa(player->GetHealth(), intChar, 10);
	std::string timeLeft = "Time Left: ";
	timeLeft += itoa((int)gameManager->getTimeLeft(), intChar, 10);
	ImGui::Begin("GGP Game", (bool*)1);
	ImGui::Text(timeLeft.c_str());
	ImGui::Text(health.c_str());
	ImGui::Text(score.c_str());
	ImGui::End();

	
	if (ImGui::BeginPopup("EndGame")) {
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Game is over");
		std::string finalScore = "Final Score: ";
		finalScore += itoa(gameManager->GetGameScore(), intChar, 10);
		ImGui::Text(finalScore.c_str());
		if (ImGui::Button("Restart Game"))
		{
			gameManager->StartGame(&assetManager, width, height, context);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
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
		Transform* pt = player->GetTransform();
		XMFLOAT3 startPt;

		XMStoreFloat3(&startPt,
			XMLoadFloat3(&pt->GetPosition()) + XMLoadFloat3(&pt->GetForward())*0.1f);

		//Make player shoot
		projectileManager->SpawnPlayerProjectile(startPt, pt->GetRotation());
	}
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

	//When the right mouse button is pressed
	//Duh, it's a bitwise &
	if (buttonState & MK_RBUTTON)
	{
		//Distance the mouse moves in one frame
		float deltaX = x - (float)prevMousePos.x;
		float deltaY = y - (float)prevMousePos.y;

		//Rotate player
		player->UpdateMouseInput(deltaX, deltaY);

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
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion