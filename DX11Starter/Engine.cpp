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
	//ShowCursor(true);
	freeMouse = true;

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
	skyBoxRasterState->Release();
	skyboxDepthStencilState->Release();

	//Release post processing resources
	ppRTV->Release();
	bloomSRV->Release();
	ppSRV->Release();

	//delete emitter;
	//Don't forget to delete the renderer
	delete renderer;

	delete font;
	delete spriteBatch;
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
	

	//Default prevMousePos to center of screen
	/*prevMousePos.x = width / 2;
	prevMousePos.y = height / 2;*/

	spriteBatch = new SpriteBatch(context);
	font = new SpriteFont(device, L"../../DX11Starter/Assets/Fonts/Arial.spritefont");

	LoadShaders();
	
	renderer = new Renderer(GameManager::getInstance().GetPlayer()->GetViewMatrix(), GameManager::getInstance().GetPlayer()->GetProjectionMatrix(), context, device, width, height, GameManager::getInstance().GetPlayer());

	CreateMaterials();
	CreateMeshes();

	gameManager->StartGame(assetManager, (float)width, (float)height, context, device); //starts the game
	
	//----------Skybox DX States ---------------//
	//rasterizer state that allows us to draw inside of cube
	D3D11_RASTERIZER_DESC rasterizerDesc = {}; // Remember to zero it out! so there is no garbage data
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.DepthClipEnable = true;
	device->CreateRasterizerState(&rasterizerDesc, &skyBoxRasterState);

	//Depth state for accepting pixels with depth equal to existing depth
	D3D11_DEPTH_STENCIL_DESC depthStencil = {};
	depthStencil.DepthEnable = true;
	depthStencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencil.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&depthStencil, &skyboxDepthStencilState);
	//END SKYBOX

	//Post processing (Bloom) states
	//Create the post processing texture
	ID3D11Texture2D* postProcessingTexture;

	//Create a texture description
	D3D11_TEXTURE2D_DESC textureDescription = {};
	textureDescription.Width = width; //Texure width (in texels)
	textureDescription.Height = height; //Texture height (in texels)
	textureDescription.ArraySize = 1; //Number of textures in the texture array
	textureDescription.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; //Binding pipeline stages
	textureDescription.CPUAccessFlags = 0; //Specifies CPU access (0 is no CPU access required)
	textureDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //Texture format
	textureDescription.MipLevels = 1; //The maximum number of mipmap levels in the texture (1 for multisampled, 0 to generate a full subset of textures)
	textureDescription.MiscFlags = 0; //Other flags, 0 is none
	textureDescription.SampleDesc.Count = 1; //Number of multisamples per pixel
	textureDescription.SampleDesc.Quality = 0; //Image quality level
	textureDescription.Usage = D3D11_USAGE_DEFAULT; //How to read from and write to the texture

	//Create the Render Target View description
	D3D11_RENDER_TARGET_VIEW_DESC rtvDescription = {};
	rtvDescription.Format = textureDescription.Format; //Texture format
	rtvDescription.Texture2D.MipSlice = 0;
	rtvDescription.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	//Create the Shader Resource View description
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDescription = {};
	srvDescription.Format = textureDescription.Format; //Texture format
	srvDescription.Texture2D.MipLevels = 1;
	srvDescription.Texture2D.MostDetailedMip = 0;
	srvDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	//Create the resources needed from the descriptions above
	device->CreateTexture2D(&textureDescription, 0, &postProcessingTexture);
	device->CreateRenderTargetView(postProcessingTexture, &rtvDescription, &ppRTV);
	device->CreateShaderResourceView(postProcessingTexture, &srvDescription, &bloomSRV);
	device->CreateShaderResourceView(postProcessingTexture, &srvDescription, &ppSRV);

	//Release the texture since it's no longer needed
	postProcessingTexture->Release();
	//END POST PROCESSING

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
	//Load default shaders
	//Textures and lighting
	SimpleVertexShader* baseVertexShader = new SimpleVertexShader(device, context);
	baseVertexShader->LoadShaderFile(L"BaseVertexShader.cso");

	SimplePixelShader* basePixelShader = new SimplePixelShader(device, context);
	basePixelShader->LoadShaderFile(L"BasePixelShader.cso");

	//Load normal mapping shaders
	//Textures, normal mapping, and lighting
	SimplePixelShader* normalPixelShader = new SimplePixelShader(device, context);
	normalPixelShader->LoadShaderFile(L"NormalMapPixelShader.cso");

	//Load Skybox Shaders
	SimpleVertexShader* skyVShader = new SimpleVertexShader(device, context);
	skyVShader->LoadShaderFile(L"SkyVertexShader.cso");

	SimplePixelShader* skyPShader = new SimplePixelShader(device, context);
	skyPShader->LoadShaderFile(L"SkyPixelShader.cso");

	//Load post processing shaders
	SimpleVertexShader* ppVShader = new SimpleVertexShader(device, context);
	ppVShader->LoadShaderFile(L"PostProcessVertexShader.cso");

	SimplePixelShader* brightnessPShader = new SimplePixelShader(device, context);
	brightnessPShader->LoadShaderFile(L"BrightnessPixelShader.cso");

	SimplePixelShader* blurPShader = new SimplePixelShader(device, context);
	blurPShader->LoadShaderFile(L"BlurPixelShader.cso");

	SimplePixelShader* bloomPShader = new SimplePixelShader(device, context);
	bloomPShader->LoadShaderFile(L"BloomPixelShader.cso");

	//Load particle shaders
	SimpleVertexShader* particleVShader = new SimpleVertexShader(device, context);
	particleVShader->LoadShaderFile(L"ParticleVertexShader.cso");

	SimplePixelShader* particlePShader = new SimplePixelShader(device, context);
	particlePShader->LoadShaderFile(L"ParticlePixelShader.cso");

	//Load shadow mapping shaders
	SimpleVertexShader* shadowVS = new SimpleVertexShader(device, context);
	shadowVS->LoadShaderFile(L"ShadowVertexShader.cso");

	//Store Vertex and Pixel Shaders into the AssetManager
	assetManager->StoreVShader("BaseVertexShader", baseVertexShader);
	assetManager->StorePShader("BasePixelShader", basePixelShader);
	assetManager->StorePShader("NormalMapPixelShader", normalPixelShader);
	assetManager->StoreVShader("SkyboxShader", skyVShader);
	assetManager->StorePShader("SkyboxShader", skyPShader);
	assetManager->StoreVShader("PostProcessVShader", ppVShader);
	assetManager->StorePShader("BrightnessPShader", brightnessPShader);
	assetManager->StorePShader("BlurPShader", blurPShader);
	assetManager->StorePShader("BloomPShader", bloomPShader);
	assetManager->StoreVShader("ParticleShader", particleVShader);
	assetManager->StorePShader("ParticleShader", particlePShader);
	assetManager->StoreVShader("ShadowShader", shadowVS);
}

// ---------------------------------------------------------
// Create meshes that GameObjects will use
// ---------------------------------------------------------
void Engine::CreateMeshes()
{
	//Create meshes
	assetManager->ImportMesh("Cone", "../../DX11Starter/Assets/Models/cone.obj", device);
	assetManager->ImportMesh("Cube", "../../DX11Starter/Assets/Models/cube.obj", device, BOX, false);
	assetManager->ImportMesh("Cylinder", "../../DX11Starter/Assets/Models/cylinder.obj", device);
	assetManager->ImportMesh("Helix", "../../DX11Starter/Assets/Models/helix.obj", device);
	assetManager->ImportMesh("Sphere", "../../DX11Starter/Assets/Models/sphere.obj", device, SPHERE, false);
	assetManager->ImportMesh("Torus", "../../DX11Starter/Assets/Models/torus.obj", device);
	assetManager->ImportMesh("Cactus", "../../DX11Starter/Assets/Models/cactus.obj", device);
	assetManager->ImportMesh("RustyPete", "../../DX11Starter/Assets/Models/RustyPete/RustyPete.obj", device, BOX, true);
	assetManager->ImportMesh("PurpleGhost", "../../DX11Starter/Assets/Models/ghost.obj", device, BOX, false);
	assetManager->ImportMesh("Plane", "../../DX11Starter/Assets/Models/Quad.obj", device, BOX, false);
	assetManager->ImportMesh("SphereHP", "../../DX11Starter/Assets/Models/sphereHP.obj", device, SPHERE, false);
	assetManager->ImportMesh("Skeleton", "../../DX11Starter/Assets/Models/skeleton.obj", device, BOX, true);
	assetManager->ImportMesh("PineTree", "../../DX11Starter/Assets/Models/pineTree.obj", device, BOX, false);
	assetManager->ImportMesh("Sign", "../../DX11Starter/Assets/Models/sign.obj", device, BOX, false);
	assetManager->ImportMesh("Lamp", "../../DX11Starter/Assets/Models/lamp.obj", device, BOX, false);
	assetManager->ImportMesh("Barrel", "../../DX11Starter/Assets/Models/barrel.obj", device, SPHERE, false);
	assetManager->ImportMesh("Campfire", "../../DX11Starter/Assets/Models/campfire.obj", device,ColliderType::NONE, false);
}

///Loads in textures and makes them into materials
void Engine::CreateMaterials()
{
	//Asset Manager Loading
	//Create Sampler State
	ID3D11SamplerState* sample;
	D3D11_SAMPLER_DESC sampleDesc = {}; //Holds options for sampling

	//Describes how to handle addresses outside 0-1 UV range
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	//Describes how to handle sampling between pixels
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX; //Mipmaps (if applicable)

	HRESULT sampleResult = device->CreateSamplerState(&sampleDesc, &sample);
	if (sampleResult != S_OK) {
		printf("Sample State could not be created");
	}

	//Particle Sampler
	ID3D11SamplerState* particleSample;
	 sampleDesc = {}; //Holds options for sampling

	//Describes how to handle addresses outside 0-1 UV range
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	sampleDesc.Filter = D3D11_FILTER_ANISOTROPIC;	//Describes how to handle sampling between pixels
	sampleDesc.MaxAnisotropy = 16;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX; //Mipmaps (if applicable)

	sampleResult = device->CreateSamplerState(&sampleDesc, &particleSample);
	if (sampleResult != S_OK) {
		printf("Sample State could not be created");
	}
	//Create the sampler object
	assetManager->StoreSampler("BasicSampler", sample);
	assetManager->StoreSampler("ParticleSampler", particleSample);

	//Create Texture
	assetManager->ImportTexture("HazardTexture", L"../../DX11Starter/Assets/Textures/HazardCrateTexture.jpg", device, context);
	assetManager->CreateMaterial("HazardCrateMat", "BaseVertexShader", "BasePixelShader", "HazardTexture", "BasicSampler");
	assetManager->ImportTexture("RustyPete", L"../../DX11Starter/Assets/Models/RustyPete/rusty_pete_body_c.png", device, context);
	assetManager->CreateMaterial("RustyPeteMaterial", "BaseVertexShader", "BasePixelShader", "RustyPete", "BasicSampler");
	assetManager->ImportTexture("Stone", L"../../DX11Starter/Assets/Textures/GreyStoneTexture.jpg", device, context);
	assetManager->CreateMaterial("StoneMat", "BaseVertexShader", "BasePixelShader", "Stone", "BasicSampler");
	assetManager->ImportTexture("PurpleGhost", L"../../DX11Starter/Assets/Textures/ghost-dark.png", device, context);
	assetManager->CreateMaterial("PurpleGhost", "BaseVertexShader", "BasePixelShader", "PurpleGhost", "BasicSampler");
	assetManager->ImportTexture("RockTexture", L"../../DX11Starter/Assets/Textures/rock.jpg", device, context);
	assetManager->ImportTexture("RockNormal", L"../../DX11Starter/Assets/Textures/rockNormals.jpg", device, context);
	assetManager->CreateMaterial("RockMaterial", "BaseVertexShader", "NormalMapPixelShader", "RockTexture", "RockNormal", "BasicSampler");
	assetManager->ImportTexture("Ground", L"../../DX11Starter/Assets/Textures/ground.jpg", device, context);
	assetManager->CreateMaterial("GroundMat", "BaseVertexShader", "BasePixelShader", "Ground", "BasicSampler");
	assetManager->ImportTexture("Skeleton", L"../../DX11Starter/Assets/Textures/skeleton.png", device, context);
	assetManager->ImportTexture("SkeletonNorm", L"../../DX11Starter/Assets/Textures/skeletonNorm.png", device, context);
	assetManager->CreateMaterial("SkeletonMat", "BaseVertexShader", "NormalMapPixelShader", "Skeleton", "SkeletonNorm", "BasicSampler");
	assetManager->ImportTexture("pineTree", L"../../DX11Starter/Assets/Textures/pineTree.png", device, context);
	assetManager->CreateMaterial("pineTreeMat", "BaseVertexShader", "BasePixelShader", "pineTree", "BasicSampler");
	assetManager->ImportTexture("brown", L"../../DX11Starter/Assets/Textures/brown.png", device, context);
	assetManager->CreateMaterial("BrownMat", "BaseVertexShader", "BasePixelShader", "brown", "BasicSampler");
	assetManager->ImportTexture("Lamp", L"../../DX11Starter/Assets/Textures/lamp.png", device, context);
	assetManager->CreateMaterial("LampMat", "BaseVertexShader", "BasePixelShader", "Lamp", "BasicSampler");
	assetManager->ImportTexture("BarrelTexture", L"../../DX11Starter/Assets/Textures/barrel.png", device, context);
	assetManager->CreateMaterial("BarrelMaterial", "BaseVertexShader", "BasePixelShader", "BarrelTexture", "BasicSampler");
	assetManager->ImportTexture("Campfire", L"../../DX11Starter/Assets/Textures/Tx_Bonfire.jpg", device, context);
	assetManager->CreateMaterial("CampfireMaterial", "BaseVertexShader", "BasePixelShader", "Campfire", "BasicSampler");

	//import crosshair texture
	assetManager->ImportTexture("Crosshairs", L"../../DX11Starter/Assets/Textures/crosshairs.png", device, context);

	//import particle texture
	assetManager->ImportTexture("ParticleTexture", L"../../DX11Starter/Assets/Textures/particle.jpg", device, context);
	assetManager->ImportTexture("FireParticleTexture", L"../../DX11Starter/Assets/Textures/fireParticle.jpg", device, context);
	//import skybox Texture
	assetManager->ImportCubeMapTexture("NightSkybox", L"../../DX11Starter/Assets/Textures/NightSkybox.dds", device);
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Engine::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	gameManager->GetPlayer()->UpdateProjectionMatrix(width, height);
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

	//Engine update Loop
	gameManager->GameUpdate(deltaTime, renderer);

	//Update game state
	if (gameManager->state == GameState::playing)
	{
		//If game is playing but end conditions have been reached, switch to end state
		if (gameManager->IsGameOver())
		{
			gameManager->state = GameState::end;
			freeMouse = true;
			ShowCursor(true);
		}
	}
	//If not in play state and player hits enter, switch to play state and reset if necessary
	else if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{

		gameManager->ResetGame(renderer);
		gameManager->StartGame(assetManager, (float)(width), (float)(height), context, device);


		gameManager->state = GameState::playing;
		freeMouse = false;
		ShowCursor(false);
	}

}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Engine::Draw(float deltaTime, float totalTime)
{
	//Get Shadow Map from light's viewpoint
	renderer->RenderShadowMap(ppRTV, depthStencilView);

	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { clear_color.x, clear_color.y, clear_color.z, clear_color.w };

	//Swap to the post process Render Target View
	context->OMSetRenderTargets(1, &ppRTV, depthStencilView);

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(ppRTV, color); //Changed backBufferRTV to ppRTV for post processing
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	//Sampler has to be passed into other shaders, so get it here once
	ID3D11SamplerState* sampler = assetManager->GetSampler("BasicSampler");

	//Draw Skybox first
	//only keeps pixels that haven't been drawn to yet (ones that have a depth of 1.0)
	ID3D11Buffer* skyVB = assetManager->GetMesh("Cube")->GetVertexBuffer();
	ID3D11Buffer* skyIB = assetManager->GetMesh("Cube")->GetIndexBuffer();

	//set the skybox Buffers
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &skyVB, &stride, &offset);
	context->IASetIndexBuffer(skyIB, DXGI_FORMAT_R32_UINT, 0);

	SimpleVertexShader* skyVS = assetManager->GetVShader("SkyboxShader");
	SimplePixelShader* skyPS = assetManager->GetPShader("SkyboxShader");

	//copy Vertex shader constant data to shader
	skyVS->SetMatrix4x4("view", gameManager->GetPlayer()->GetViewMatrix());
	skyVS->SetMatrix4x4("projection", gameManager->GetPlayer()->GetProjectionMatrix());
	skyVS->CopyAllBufferData();
	skyVS->SetShader();

	//copy pixel shader constant data to shader
	skyPS->SetShaderResourceView("skyboxTexture", assetManager->GetTexture("NightSkybox"));
	skyPS->SetSamplerState("skySampler", sampler);
	skyPS->CopyAllBufferData();
	skyPS->SetShader();

	//Set skybox render state options
	context->RSSetState(skyBoxRasterState);
	context->OMSetDepthStencilState(skyboxDepthStencilState, 0);

	//render sky box
	context->DrawIndexed(assetManager->GetMesh("Cube")->GetIndexCount(), 0, 0);

	//reset render state options
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
	//END SKYBOX

	//Game drawing
	// 1. Show a simple window
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
	{
		static float f[3];
		static int arrayPos = 0;
		static char testText = char();
		ImGui::InputInt("ArrayPos", &arrayPos);
		ImGui::Text("Hello, world!");
		ImGui::InputFloat3("gameobj Pos", f);
		//gameManager->gameObjects[arrayPos]->GetTransform()->SetPosition(XMFLOAT3(f[0], f[1], f[2])); //gameObjects is now private
		ImGui::ColorEdit3("clear color", (float*)&clear_color);
		ImGui::InputText("Text Test", &testText, sizeof(char) * 50);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	gameManager->GameDraw(renderer);
	//END GAME DRAWING

	//Begin post processing
	//Start by calculating brightness, then blur and bloom
	context->OMSetRenderTargets(1, &backBufferRTV, 0); //Set the back buffer as the render target
	context->ClearRenderTargetView(backBufferRTV, color);

	//Get the shaders
	SimpleVertexShader* ppVS = assetManager->GetVShader("PostProcessVShader");
	SimplePixelShader* ppPS = assetManager->GetPShader("BrightnessPShader");

	//Set the shaders
	//Begin brightness
	ppVS->SetShader();
	ppPS->SetShader();

	//ppPS->CopyAllBufferData(); //Don't need to do this, since no data has been added

	ppPS->SetShaderResourceView("Pixels", bloomSRV); //Pass in a copy of the rendered texture to pick bright pixels from for bloom calculations
	ppPS->SetSamplerState("Sampler", sampler);
	//END BRIGHTNESS

	//Begin bloom
	//Includes blur
	ppPS = assetManager->GetPShader("BloomPShader");

	//Set the shaders
	ppPS->SetShader();

	//Send some extra data to the pixel shader
	ppPS->SetFloat("pixelWidth", 1.0f / width);
	ppPS->SetFloat("pixelHeight", 1.0f / height);
	ppPS->SetInt("blurAmount", 2); //Adjust number for more/less blur/framerate
	ppPS->CopyAllBufferData();

	ppPS->SetShaderResourceView("Render", ppSRV);
	//END BLOOM

	//Turn off vertex and index buffers because 
	//the post processing vertex shader doesn't need them
	ID3D11Buffer* noBuffer = 0;
	context->IASetVertexBuffers(0, 1, &noBuffer, &stride, &offset);
	context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

	//Draw exactly three vertices to cover the render area
	//One big-ass triangle to rule them all
	context->Draw(3, 0);

	//Turn off all SRVs to avoid any potential resource 
	//input/output issues
	ID3D11ShaderResourceView* nullSRVs[16] = {};
	context->PSSetShaderResources(0, 16, nullSRVs);
	//END POST PROCESSING

	if (ImGui::BeginPopup("EndGame")) {
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "Game is over");
		std::string finalScore = "Final Score: ";
		finalScore += to_string(gameManager->GetGameScore());
		ImGui::Text(finalScore.c_str());
		if (ImGui::Button("Restart Game"))
		{
			gameManager->ResetGame(renderer); //Clean up memory
			gameManager->StartGame(assetManager, (float)(width), (float)(height), context, device);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::Begin("UI Instructions", (bool*)1);
	ImGui::Text("Right click to free the mouse for interacting with UI");
	ImGui::End();

	ImGui::Render();

	//Draw UI with sprite fonts/crosshair
	//Get font texture
	DrawUI();

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}

void Engine::DrawUI()
{
 	ID3D11ShaderResourceView* fontTexture;
	font->GetSpriteSheet(&fontTexture);

	CommonStates states(device);

	spriteBatch->Begin(SpriteSortMode_Deferred, states.NonPremultiplied());//Non-premultiplied blend state allows us to use transparency in pngs

	if (gameManager->state == GameState::playing)
	{
		//Construct strings based on game data
		std::wstring timeString = L"Time: " + std::to_wstring((int)gameManager->getTimeLeft());
		std::wstring healthString = L"Health: " + std::to_wstring(gameManager->GetPlayer()->GetHealth());
		std::wstring scoreString = L"Score: " + std::to_wstring(gameManager->GetGameScore());

		//Origin for score text so that it's on the upper right corner
		XMFLOAT2 scoreOrigin;
		XMStoreFloat2(&scoreOrigin, font->MeasureString(scoreString.c_str()));
		scoreOrigin.y = 0;

		float crosshairSize = 24; //Crosshair is 24px x 24px

		//Get coords to draw crosshair so it's centered
		float crosshairLeft = (width - crosshairSize) / 2;
		float crosshairTop = (height - crosshairSize) / 2;

		//Rect for bounds where crosshair will be drawn
		RECT crosshairRect = { (long)crosshairLeft, (long)crosshairTop, (long)(crosshairLeft + crosshairSize), (long)(crosshairTop + crosshairSize) };

		spriteBatch->Draw(assetManager->GetTexture("Crosshairs"), crosshairRect);//Draw crosshair

		//Draw text
		font->DrawString(
			spriteBatch,
			timeString.c_str(),
			XMFLOAT2(10, 10),
			Colors::White,
			0,
			XMFLOAT2(0, 0),
			XMFLOAT2(.7f, .7f)
		);
		font->DrawString(
			spriteBatch,
			healthString.c_str(),
			XMFLOAT2(10, 35),
			Colors::White,
			0,
			XMFLOAT2(0, 0),
			XMFLOAT2(.7f, .7f)
		);
		font->DrawString(
			spriteBatch,
			scoreString.c_str(),
			XMFLOAT2((float)(width - 10), 10),
			Colors::White,
			0,
			scoreOrigin,
			XMFLOAT2(.7f, .7f)
		);
	}
	else if (gameManager->state == GameState::end)
	{
		std::wstring gameOverString = L"Game Over!";
		XMFLOAT2 gameOverOrigin;
		XMStoreFloat2(&gameOverOrigin, font->MeasureString(gameOverString.c_str())/2);

		std::wstring scoreString = L"Final score:" + std::to_wstring(gameManager->GetGameScore());
		XMFLOAT2 scoreOrigin;
		XMStoreFloat2(&scoreOrigin, font->MeasureString(scoreString.c_str())/2);

		std::wstring restartString = L"Press enter to restart";
		XMFLOAT2 restartOrigin;
		XMStoreFloat2(&restartOrigin, font->MeasureString(restartString.c_str())/2);

		font->DrawString(
			spriteBatch,
			gameOverString.c_str(),
			XMFLOAT2((float)(width / 2), (float)(height / 2 -70)),
			Colors::White,
			0,
			gameOverOrigin,
			XMFLOAT2(1.25f,1.25f)
		);

		font->DrawString(
			spriteBatch,
			scoreString.c_str(),
			XMFLOAT2((float)(width / 2), (float)(height / 2 - 30)),
			Colors::White,
			0,
			scoreOrigin
		);

		font->DrawString(
			spriteBatch,
			restartString.c_str(),
			XMFLOAT2((float)(width / 2), (float)(height / 2 + 20)),
			Colors::White,
			0,
			restartOrigin,
			XMFLOAT2(.7f, .7f)
		);
	}
	else
	{
		std::wstring titleString = L"Spook 'em Up";
		XMFLOAT2 titleOrigin;
		XMStoreFloat2(&titleOrigin, font->MeasureString(titleString.c_str()) / 2);

		std::wstring startString = L"Press enter to start";
		XMFLOAT2 startOrigin;
		XMStoreFloat2(&startOrigin, font->MeasureString(startString.c_str()) / 2);

		font->DrawString(
			spriteBatch,
			titleString.c_str(),
			XMFLOAT2((float)(width / 2), (float)(height / 2 - 100)),
			Colors::White,
			0,
			titleOrigin,
			XMFLOAT2(1.25f, 1.25f)
		);

		font->DrawString(
			spriteBatch,
			startString.c_str(),
			XMFLOAT2((float)(width / 2), (float)(height / 2 + 20)),
			Colors::White,
			0,
			startOrigin,
			XMFLOAT2(.7f, .7f)
		);
	}

	//End UI drawing
	spriteBatch->End();

	fontTexture->Release();

	// Reset any and all render states that sprite batch has changed
	// Reset any states we've changed for the next frame!
	float blendFactors[4] = { 1,1,1,1 };
	context->OMSetBlendState(0, blendFactors, 0xFFFFFFFF);
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Engine::OnMouseDown(WPARAM buttonState, int x, int y)
{
	/*if (buttonState & MK_RBUTTON)
	{
		freeMouse = !freeMouse;
		ShowCursor(freeMouse);
	}*/

	if (!freeMouse && buttonState & MK_LBUTTON)
	{
		gameManager->OnLeftClick();
	}
	
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Engine::OnMouseUp(WPARAM buttonState, int x, int y)
{
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Engine::OnMouseMove(int x, int y)
{
	if (freeMouse) return;

	if (gameManager->state == GameState::playing) {
		gameManager->GetPlayer()->UpdateMouseInput((float)x, (float)y);//Rotate player
	}

	SetCursorPos(screen.right / 2, screen.bottom / 2);
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