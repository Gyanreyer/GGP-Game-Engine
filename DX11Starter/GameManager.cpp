#include "GameManager.h"
//ImGui Includes//////////////
#include "Imgui\imconfig.h"
#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_dx11.h"
#include "Imgui\imgui_internal.h"
#include "Imgui\stb_rect_pack.h"
#include "Imgui\stb_textedit.h"
#include "Imgui\stb_truetype.h"
/////////////////////////////

GameManager::GameManager()
{
	score = 0;
	state = GameState::start;
}

GameManager & GameManager::getInstance()
{
	static GameManager instance;
	return instance;
}

GameManager::~GameManager()
{
	ClearObjects();
	delete campfireEmitter;
}

void GameManager::StartGame(AssetManager * asset, float screenWidth, float screenHeight, ID3D11DeviceContext* context, ID3D11Device* device)
{
	time(&nowTime); //gets current time when game is launched
	gameStartTime = *localtime(&nowTime); //assigns that time to gameStartTime to keep track of the time when game first started
	timeInMatch = 20; //intializes how much time is in a game
	score = 0; //sets score to 0

	spacePartitionHead = new OctreeNode(XMFLOAT3(0, -20, 0), 500, nullptr);//Will have to discuss size of play area, for now 1000x1000
	//Experimenting but I set y to -20 so that stuff on the ground won't be interpreted as on an edge
	
	//PROJECTILE MANAGER
	projectileManager = new ProjectileManager(asset->GetMesh("Sphere"),
		asset->GetMaterial("HazardCrateMat"), //Placeholder until make new mats for bullets
		asset->GetMaterial("PurpleGhost"),
		context,
		spacePartitionHead);

	//PLAYER
	player = Player(
		Transform(XMFLOAT3(0, 0, -5),//Init position
			XMFLOAT3(0, 0, 0),//Init rot
			XMFLOAT3(0.5f, 1, 0.5f)),//Init scale
		(unsigned int)screenWidth, (unsigned int)screenHeight,//Screen dimensions for projection matrix
		projectileManager);//Reference to proj manager for shooting

	CreateGameObjects(asset, context, device);
	InitSpatialPartition();
}

// --------------------------------------------------------
// Creates the GameObjects we will draw in the scene and
// stores references to them in an array
// --------------------------------------------------------
void GameManager::CreateGameObjects(AssetManager * asset, ID3D11DeviceContext* context, ID3D11Device* device)
{
	//ENEMIES
	enemies.clear(); //Clear this out for new game instances
	
	//Create a transform for the enemies
	Transform enemyTransform = Transform(
		XMFLOAT3(2, 0, 0), //Position
		XMFLOAT3(0, 0, 0), //Rotation
		XMFLOAT3(1, 1, 1) //Scale
	);

	//Create enemies
	enemies.push_back(new Enemy(enemyTransform, asset->GetMesh("RustyPete"), asset->GetMaterial("RustyPeteMaterial"), EnemyType::moveX, 20, projectileManager));
	enemyTransform.SetPosition(-2, 2, 0);
	enemies.push_back(new Enemy(enemyTransform, asset->GetMesh("PurpleGhost"), asset->GetMaterial("PurpleGhost"), EnemyType::moveY, 30, projectileManager));
	enemyTransform.SetPosition(0, 0, -2);
	enemies.push_back(new Enemy(enemyTransform, asset->GetMesh("RustyPete"), asset->GetMaterial("RustyPeteMaterial"), EnemyType::noMove, 10, projectileManager));
	enemyTransform.SetPosition(1, 0, 5);
	enemies.push_back(new Enemy(enemyTransform, asset->GetMesh("Skeleton"), asset->GetMaterial("SkeletonMat"), EnemyType::noMove, 10, projectileManager));

	///OTHER GAMEOBJECTS
	gameObjects.clear(); //Clear this out for new game instances

	//Clear instanced GameObjects out for new game instances
	trees.clear();
	lamps.clear();
	barrels.clear();

	//Store references to all GOs in vector
	gameObjects.push_back(new GameObject(Transform(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(30, 0.001f, 30)),
		asset->GetMesh("Plane"), asset->GetMaterial("GroundMat"), "Floor"));
	//Signs
	gameObjects.push_back(new GameObject(Transform(XMFLOAT3(-8.0f, 0, -1), XMFLOAT3(0, -XM_PIDIV4, 0), XMFLOAT3(1, 1, 1)),
		asset->GetMesh("Sign"), asset->GetMaterial("BrownMat"), "Obstacle"));

	//Large trees (6)
	trees.push_back(new GameObject(Transform(XMFLOAT3(-2, 1.25f, 12), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(10, 1.25f, 7), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(-5, 1.25f, 2), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(-4, 1.25f, -14), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(4, 1.25f, -10), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(6, 1.25f, -2), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	//Medium trees (12)
	trees.push_back(new GameObject(Transform(XMFLOAT3(-7, 1, -7), XMFLOAT3(0, 0, 0), XMFLOAT3(.75f, .75f, .75f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(-7.5f, 1, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(.75f, .75f, .75f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(11, 1, -4), XMFLOAT3(0, 0, 0), XMFLOAT3(.75f, .75f, .75f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(9, 1, -8), XMFLOAT3(0, 0, 0), XMFLOAT3(.75f, .75f, .75f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(-11, 1, -3), XMFLOAT3(0, 0, 0), XMFLOAT3(.75f, .75f, .75f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(12, 1, 1), XMFLOAT3(0, 0, 0), XMFLOAT3(.75f, .75f, .75f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(4, 1, 10), XMFLOAT3(0, 0, 0), XMFLOAT3(.75f, .75f, .75f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(-2, 1, 12), XMFLOAT3(0, 0, 0), XMFLOAT3(.75f, .75f, .75f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(9, 1, 2.5f), XMFLOAT3(0, 0, 0), XMFLOAT3(.75f, .75f, .75f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(-9, 1, 4), XMFLOAT3(0, 0, 0), XMFLOAT3(.75f, .75f, .75f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(-12, 1, -12), XMFLOAT3(0, 0, 0), XMFLOAT3(.75f, .75f, .75f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(-1, 1, -10), XMFLOAT3(0, 0, 0), XMFLOAT3(.75f, .75f, .75f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	//Small trees (9)
	trees.push_back(new GameObject(Transform(XMFLOAT3(-.5f, .55f, 5), XMFLOAT3(0, 0, 0), XMFLOAT3(.4f, .4f, .4f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(5, .55f, 3), XMFLOAT3(0, 0, 0), XMFLOAT3(.4f, .4f, .4f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(0, .55f, -13), XMFLOAT3(0, 0, 0), XMFLOAT3(.4f, .4f, .4f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(-3, .55f, -8), XMFLOAT3(0, 0, 0), XMFLOAT3(.4f, .4f, .4f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(2, .55f, -6), XMFLOAT3(0, 0, 0), XMFLOAT3(.4f, .4f, .4f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(6, .55f, -6), XMFLOAT3(0, 0, 0), XMFLOAT3(.4f, .4f, .4f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(-6, .55f, 7), XMFLOAT3(0, 0, 0), XMFLOAT3(.4f, .4f, .4f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(-7, .55f, 9), XMFLOAT3(0, 0, 0), XMFLOAT3(.4f, .4f, .4f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	trees.push_back(new GameObject(Transform(XMFLOAT3(10, .55f, 10), XMFLOAT3(0, 0, 0), XMFLOAT3(.4f, .4f, .4f)),
		asset->GetMesh("PineTree"), asset->GetMaterial("pineTreeMat"), "Obstacle"));
	//Lamps (2)
	lamps.push_back(new GameObject(Transform(XMFLOAT3(4, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1)),
		asset->GetMesh("Lamp"), asset->GetMaterial("LampMat"), "Obstacle"));
	lamps.push_back(new GameObject(Transform(XMFLOAT3(-5.5f, 0, -3.5f), XMFLOAT3(0, XM_PI, 0), XMFLOAT3(1, 1, 1)),
		asset->GetMesh("Lamp"), asset->GetMaterial("LampMat"), "Obstacle"));
	//Barrels (3)
	barrels.push_back(new GameObject(Transform(XMFLOAT3(3.9f, 0, -.4f), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1)),
		asset->GetMesh("Barrel"), asset->GetMaterial("BarrelMaterial"), "Obstacle"));
	barrels.push_back(new GameObject(Transform(XMFLOAT3(-3.9f, 0, -1), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1)),
		asset->GetMesh("Barrel"), asset->GetMaterial("BarrelMaterial"), "Obstacle"));
	barrels.push_back(new GameObject(Transform(XMFLOAT3(-3.3f, 0, -.8f), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1)),
		asset->GetMesh("Barrel"), asset->GetMaterial("BarrelMaterial"), "Obstacle"));

	//Fire
	gameObjects.push_back(new GameObject(Transform(XMFLOAT3(2.0f, 0.2f, -2), XMFLOAT3(0, -XM_PIDIV4, 0), XMFLOAT3(1, 1, 1)),
		asset->GetMesh("Campfire"), asset->GetMaterial("CampfireMaterial"), "Fire"));
	campfireEmitter = new Emitter(
		100,
		20,
		0.4,
		0.1f,
		1.0f,
		XMFLOAT4(1, 0.1f, 0.1f, 1.0f),	// Start color
		XMFLOAT4(1, 0.6f, 0.1f, 0.0f),		// End color
		XMFLOAT3(0, .5, 0),				// Start velocity
		XMFLOAT3(2.0f, 0.2f, -2),				// Start position
		XMFLOAT3(0, -.01, 0),				// Start acceleration
		asset->GetVShader("ParticleShader"),
		asset->GetPShader("ParticleShader"),
		asset->GetTexture("FireParticleTexture"),
		device);
}

void GameManager::InitSpatialPartition()
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		spacePartitionHead->AddObject(gameObjects[i]);
	}

	for (int i = 0; i < enemies.size(); i++)
	{
		spacePartitionHead->AddObject(enemies[i]);
	}

	spacePartitionHead->AddObject(&player);
}

void GameManager::CheckObjectCollisions(float deltaTime)
{
	//Check collisions for projectiles with everything
	vector<Projectile *> projs = projectileManager->GetProjectiles();

	for (vector<Projectile *>::iterator projIter = projs.begin(); projIter != projs.end(); ++projIter)
	{
		//Get objects to check collision with
		vector<GameObject *> projOctObjects = (*projIter)->GetOctNode()->GetAllObjectsForCollision();

		for (vector<GameObject *>::iterator otherIter = projOctObjects.begin(); otherIter != projOctObjects.end(); ++otherIter)
		{
			const char* tag = (*otherIter)->GetTag();//Get object's tag so we know how to handle collision

			//Check collision and destory projectile if it hits anything
			//Ignore collision with other projectiles
			if (strcmp(tag, "Projectile") != 0 && Collision::CheckCollision((*otherIter)->GetCollider(), (*projIter)->GetCollider()))
			{	
				//Check tag to determine what else to do
				if (strcmp(tag, "Player") == 0)
				{
					player.DecrementHealth();

					XMFLOAT3 hitForce = (*projIter)->GetTransform()->GetForward();

					hitForce.y = 1;

					player.Accelerate(hitForce);
				}
				else if (strcmp(tag, "Enemy") == 0)
				{
					vector<Enemy *>::iterator enemyIter = std::find(enemies.begin(), enemies.end(), *otherIter);

					AddScore((*enemyIter)->GetPoints());//Add points to score
					(*enemyIter)->GetOctNode()->RemoveObject(*enemyIter);//Remove enemy from octree
					enemies.erase(enemyIter); //Remove enemy from the vector
					delete *otherIter; //Destroy the enemy
				}

				projectileManager->RemoveProjectile(*projIter); //Destroy projectile

				break; //Break out of the loop, otherwise errors arise from destroying the projectile
			}
		}
	}

	//Check collisions for player
	vector<GameObject *> playerOctObjects = player.GetOctNode()->GetAllObjectsForCollision();

	for (vector<GameObject *>::iterator otherIter = playerOctObjects.begin(); otherIter != playerOctObjects.end(); ++otherIter)
	{
		const char* tag = (*otherIter)->GetTag();//Get object's tag so we know how to handle collision

		if (strcmp(tag, "Floor") == 0 &&
			Collision::CheckCollision((*otherIter)->GetCollider(), player.GetCollider()))
		{
			player.StopFalling(0);
		}

		//If an object or enemy, the player should collide and be unable to pass through them
		else if ((strcmp(tag, "Obstacle") == 0 || strcmp(tag, "Enemy") == 0 ) &&
			Collision::CheckCollision((*otherIter)->GetCollider(), player.GetCollider()))
		{
			XMFLOAT3 playerVelocity = player.GetVelocity();

			Collider* otherColl = (*otherIter)->GetCollider();

			XMFLOAT3 vecToPlayer;

			//Get vector from other object's center to player
			XMStoreFloat3(&vecToPlayer,
				XMVector3Normalize(XMLoadFloat3(&player.GetCollider()->center) - XMLoadFloat3(&otherColl->center)));

			//Resolve collision
			if (otherColl->collType == BOX)
			{
				//Get absolute value vector so we can see which component is longest
				XMFLOAT3 absVecToObject(fabs(vecToPlayer.x), fabs(vecToPlayer.y), fabs(vecToPlayer.z));

				//If player is jumping/moving up away from the box, don't do anything
				if (absVecToObject.y >= absVecToObject.x && absVecToObject.y >= absVecToObject.z &&
					player.GetCollider()->center.y >= otherColl->center.y)
				{
					//Snap to top of box
					player.StopFalling(otherColl->center.y + otherColl->dimensions.y);
				}
				//Cancel out the player's movement into the box based on what side they're on
				else
				{
					if (absVecToObject.x > absVecToObject.z)
					{
						playerVelocity.x = 0;
					}
					else
					{
						playerVelocity.z = 0;
					}

					//Set new modified velocity
					player.SetVelocity(playerVelocity);
				}
			}
			else
			{
				//This doesn't work properly yet will figure out later
			}
		}	
	}
}

void GameManager::OnLeftClick()
{
	if(state == GameState::playing)
		player.Shoot();
}

void GameManager::GameUpdate(float deltaTime)
{
	//1. Make sure game is has not ended
	if (state == GameState::playing) {
		campfireEmitter->Update(deltaTime);

		player.UpdatePhysics(deltaTime);

		//Update the player
		player.Update(deltaTime);

		//Update all projectiles
		projectileManager->UpdateProjectiles(deltaTime);

		//Get the player position now
		XMFLOAT3 playerPos = player.GetTransform()->GetPosition();

		//Get and update enemies
		for (int i = 0; i < enemies.size(); i++)
		{
			enemies[i]->Update(deltaTime); //Update enemies

			//Enemy shooting
			XMFLOAT3 enemyPos = enemies[i]->GetTransform()->GetPosition(); //Get the enemy position

			float distance = sqrt(pow((playerPos.x - enemyPos.x), 2) + pow((playerPos.y - enemyPos.y), 2) + pow((playerPos.z - enemyPos.z), 2)); //Distance from the player to the enemy

			time(enemies[i]->GetNowTime()); //get current time in game
			double seconds = difftime(*enemies[i]->GetNowTime(), mktime(enemies[i]->GetLastShotTime()));

			if (distance < 10) //If the enemy should shoot and the player is in range
			{
				XMFLOAT3 vecToPlayer;

				XMStoreFloat3(&vecToPlayer, XMVector3Normalize(XMLoadFloat3(&player.GetCollider()->center) - XMLoadFloat3(&enemies[i]->GetCollider()->center)));

				enemies[i]->GetTransform()->SetForward(vecToPlayer);

				if(seconds >= 4)
					enemies[i]->Shoot();
			}
		}

		//Update what nodes objects are stored in, delete unnecessary ones
		spacePartitionHead->UpdateAll();

		CheckObjectCollisions(deltaTime);//Check all collisions		
	}
}

void GameManager::GameDraw(Renderer* renderer)
{
	renderer->SetViewProjMatrix(player.GetViewMatrix(), player.GetProjectionMatrix());

	//Render instanced gameobjects
	renderer->RenderInstanced(trees);
	renderer->RenderInstanced(lamps);
	renderer->RenderInstanced(barrels);

	//Loop through GameObjects and draw them
	for (byte i = 0; i < gameObjects.size(); i++)
	{
		renderer->Render(gameObjects[i]);
	}

	//Loop through Enemies and draw them
	for (byte i = 0; i < enemies.size(); i++)
	{
		renderer->Render(enemies[i]);
	}

	//Draw all projectiles
	projectileManager->DrawProjectiles(renderer);

	renderer->Render(campfireEmitter);

	/*
	//Display game stats
	std::string score = "Score: ";
	score += to_string(GetGameScore());
	std::string health = "Health: ";
	health += to_string(player.GetHealth());
	std::string timeLeft = "Time Left: ";
	timeLeft += to_string((int)getTimeLeft());
	ImGui::Begin("GGP Game", (bool*)1);
	ImGui::Text(timeLeft.c_str());
	ImGui::Text(health.c_str());
	ImGui::Text(score.c_str());
	ImGui::End();
	*/
}

Player * GameManager::GetPlayer()
{
	return &player;
}

double GameManager::getTimeLeft()
{
	time(&nowTime);
	
	double seconds = difftime(nowTime, mktime(&gameStartTime));
	return timeInMatch - seconds;
}

void GameManager::AddScore(int addAmount)
{
	score += addAmount;
}

void GameManager::ResetGame()
{
	time(&nowTime); //gets current time when game is launched
	gameStartTime = *localtime(&nowTime); //assigns that time to gameStartTime to keep track of the time when game first started
	score = 0; //sets score to 0

	ClearObjects();
	delete campfireEmitter;
}

bool GameManager::IsGameOver()
{
	return getTimeLeft() <= 0 || player.GetHealth() == 0 || enemies.size() == 0;
}

int GameManager::GetGameScore()
{
	return score;
}

void GameManager::ClearObjects()
{
	//Delete enemies
	for (byte i = 0; i < enemies.size(); i++)
	{
		delete enemies[i];
	}

	enemies.clear();

	//Delete gameobjects
	for (byte i = 0; i < gameObjects.size(); i++)
	{
		delete gameObjects[i];
	}

	gameObjects.clear();

	//Delete instanced gameobjects
	for (byte i = 0; i < trees.size(); i++)
	{
		delete trees[i];
	}

	trees.clear();

	for (byte i = 0; i < lamps.size(); i++)
	{
		delete lamps[i];
	}

	lamps.clear();

	for (byte i = 0; i < barrels.size(); i++)
	{
		delete barrels[i];
	}

	barrels.clear();

	delete projectileManager;
	delete spacePartitionHead;
}


