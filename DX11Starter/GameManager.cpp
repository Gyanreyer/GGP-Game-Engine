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
}

GameManager & GameManager::getInstance()
{
	static GameManager instance;
	return instance;
}

GameManager::~GameManager()
{
	ClearObjects();
}

void GameManager::StartGame(AssetManager * asset, float screenWidth, float screenHeight, ID3D11DeviceContext* context)
{
	time(&nowTime); //gets current time when game is launched
	gameStartTime = *localtime(&nowTime); //assigns that time to gameStartTime to keep track of the time when game first started
	timeInMatch = 99; //intializes how much time is in a game
	score = 0; //sets score to 0

	spacePartitionHead = OctreeNode(XMFLOAT3(0, -20, 0), 500, nullptr);//Will have to discuss size of play area, for now 1000x1000
	//Experimenting but I set y to -20 so that stuff on the ground won't be interpreted as on an edge
	
	//PROJECTILE MANAGER
	projectileManager = ProjectileManager(asset->GetMesh("Sphere"),
		asset->GetMaterial("HazardCrateMat"), //Placeholder until make new mats for bullets
		asset->GetMaterial("PurpleGhost"),
		context,
		&spacePartitionHead);

	//PLAYER
	player = Player(
		Transform(XMFLOAT3(0, 1, -5),//Init position
			XMFLOAT3(0, 0, 0),//Init rot
			XMFLOAT3(0.5f, 1, 0.5f)),//Init scale
		(unsigned int)screenWidth, (unsigned int)screenHeight,//Screen dimensions for projection matrix
		&projectileManager);//Reference to proj manager for shooting

	CreateGameObjects(asset, context);
	InitSpatialPartition();
}

// --------------------------------------------------------
// Creates the GameObjects we will draw in the scene and
// stores references to them in an array
// --------------------------------------------------------
void GameManager::CreateGameObjects(AssetManager * asset, ID3D11DeviceContext* context)
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
	enemies.push_back(new Enemy(enemyTransform, asset->GetMesh("RustyPete"), asset->GetMaterial("RustyPeteMaterial"), EnemyType::moveX, 10, &projectileManager));
	enemyTransform.SetPosition(-2, 2, 0);
	enemies.push_back(new Enemy(enemyTransform, asset->GetMesh("PurpleGhost"), asset->GetMaterial("PurpleGhost"), EnemyType::moveY, 20, &projectileManager));
	enemyTransform.SetPosition(0, 0, -2);
	enemies.push_back(new Enemy(enemyTransform, asset->GetMesh("RustyPete"), asset->GetMaterial("RustyPeteMaterial"), EnemyType::moveX, 20, &projectileManager ));
	enemyTransform.SetPosition(0, 1, 0);
	enemies.push_back(new Enemy(enemyTransform, asset->GetMesh("SphereHP"), asset->GetMaterial("RockMaterial"), EnemyType::moveX, 20, &projectileManager )); //DIRTY BUBBLE!
	

	///OTHER GAMEOBJECTS
	gameObjects.clear(); //Clear this out for new game instances

	//Store references to all GOs in vector
	gameObjects.push_back(new GameObject(Transform(XMFLOAT3(0,0,0),XMFLOAT3(0,0,0),XMFLOAT3(10, 0.001f, 10)),
		asset->GetMesh("Plane"), asset->GetMaterial("RustyPeteMaterial"),"Floor"));
	gameObjects.push_back(new GameObject(Transform(XMFLOAT3(4,0.5f,-2),XMFLOAT3(0,0,0),XMFLOAT3(1,1,1)),
		asset->GetMesh("Cube"), asset->GetMaterial("RockMaterial"),"Obstacle"));
	//gameObjects.push_back(GameObject(Transform(XMFLOAT3(2, 1, -2), XMFLOAT3(0, 0, 0), XMFLOAT3(2, 2, 2)),
	//	asset->GetMesh("Cube"), asset->GetMaterial("StoneMat")));
	//gameObjects.push_back(GameObject(Transform(XMFLOAT3(2,0.25f,-2),XMFLOAT3(0,0,0),XMFLOAT3(0.5f,0.5f,0.5f)),
	//	asset->GetMesh("Cube"), asset->GetMaterial("StoneMat")));
	//gameObjects.push_back(GameObject(Transform(XMFLOAT3(-2, 0.25f, -2),XMFLOAT3(0,0,0),XMFLOAT3(0.5f,0.5f,0.5f)),
	//	asset->GetMesh("Sphere"), asset->GetMaterial("StoneMat")));
}

void GameManager::InitSpatialPartition()
{
	for (int i = 0; i < gameObjects.size(); i++)
	{
		spacePartitionHead.AddObject(gameObjects[i]);
	}

	for (int i = 0; i < enemies.size(); i++)
	{
		printf("Enemy pointer: %p\n", enemies[i]);
		spacePartitionHead.AddObject(enemies[i]);
	}

	spacePartitionHead.AddObject(&player);
}

void GameManager::CheckObjectCollisions(float deltaTime)
{
	//Check collisions for player
	vector<GameObject *> playerOctObjects = player.GetOctNode()->GetAllObjectsForCollision();
	
	for (vector<GameObject *>::iterator iterColl = playerOctObjects.begin(); iterColl != playerOctObjects.end();++iterColl)
	{
		const char* tag = (*iterColl)->GetTag();//Get object's tag so we know how to handle collision

		if (strcmp(tag, "Player") == 0) continue;//Skip the player, no need to check against self

		//If projectile, check for collision and take damage if hit
		else if (strcmp(tag, "Projectile") == 0 && Collision::CheckCollisionSphereBox((*iterColl)->GetCollider(), player.GetCollider()))
		{
			player.DecrementHealth();
			projectileManager.RemoveProjectileByAddress(*iterColl);//Destroy projectile
		}
		//If an object or enemy, the player should collide and be unable to pass through them
		else if ((strcmp(tag, "Obstacle") == 0 || strcmp(tag, "Enemy") == 0 ) &&
			Collision::CheckCollision((*iterColl)->GetCollider(), player.GetCollider()))
		{
			Transform* playerTrans = player.GetTransform();

			XMFLOAT3 vecToPlayer;

			//Get vector from other object's center to player
			XMStoreFloat3(&vecToPlayer, XMLoadFloat3(&player.GetCollider()->center) - XMLoadFloat3(&(*iterColl)->GetCollider()->center));

			XMFLOAT3 playerVelocity = playerTrans->GetVelocity();

			Collider* otherColl = (*iterColl)->GetCollider();

			//Resolve collision
			if (otherColl->collType == BOX)
			{
				//Get absolute value vector so we can see which component is longest
				XMFLOAT3 absVecToObject(fabs(vecToPlayer.x), fabs(vecToPlayer.y), fabs(vecToPlayer.z));

				//Cancel out the player's movement into the box based on what side they're on
				if (absVecToObject.x >= absVecToObject.y && absVecToObject.x >= absVecToObject.z)
				{
					playerVelocity.x = 0;
				}
				else if (absVecToObject.z >= absVecToObject.x && absVecToObject.z >= absVecToObject.y)
				{
					playerVelocity.z = 0;
				}
				//If player is jumping/moving up away from the box, don't do anything
				else if(playerVelocity.y <= 0)
				{
					playerVelocity.y = 0;

					//Snap to top of box
					XMFLOAT3 pos = playerTrans->GetPosition();
					pos.y = otherColl->center.y + otherColl->dimensions.y + player.GetCollider()->dimensions.y;
	
					playerTrans->SetPosition(pos);

					player.SetIsOnGO(true);//Indicate player landed on box so they can jump
				}

				//Set new modified velocity
				playerTrans->SetVelocity(playerVelocity);
			}
			else
			{
				//Let's do some goddamn math, WOOOOO
				//Well, tomorrow because I should sleep
				//But this will get a projection of the vector to the player
				//onto the inverse of the player's velocity vector, and then
				//we'll apply as a force to cancel movement towards the sphere's center
			}
		}	
	}

	//Check collisions between enemies and projectiles
	for (vector<Enemy *>::iterator enemyIter = enemies.begin(); enemyIter != enemies.end();)
	{
		byte incAmt = 1;

		vector<GameObject *> enemyOctObjects = (*enemyIter)->GetOctNode()->GetAllObjectsForCollision();

		for (vector<GameObject *>::iterator iterColl = enemyOctObjects.begin(); iterColl != enemyOctObjects.end();++iterColl)
		{
			const char* tag = (*iterColl)->GetTag();//Get object's tag so we know how to handle collision

			//If projectile, check for collision and die if hit
			if (strcmp(tag, "Projectile") == 0 && Collision::CheckCollision((*iterColl)->GetCollider(), (*enemyIter)->GetCollider()))
			{
				AddScore((*enemyIter)->GetPoints());//Add points to score
				(*enemyIter)->GetOctNode()->RemoveObject(*enemyIter._Ptr);//Remove enemy from octree
				enemyIter = enemies.erase(enemyIter);//Destroy enemy
				projectileManager.RemoveProjectileByAddress(*iterColl);//Destroy projectile
				incAmt = 0;
				break;
			}
		}

		enemyIter += incAmt;
	}

	//Check collisions for projectiles with objects
	vector<Projectile *> projs = projectileManager.GetProjectiles();

	for (vector<Projectile *>::iterator projIter = projs.begin(); projIter != projs.end(); ++projIter)
	{
		vector<GameObject *> projOctObjects = (*projIter)->GetOctNode()->GetAllObjectsForCollision();

		for (vector<GameObject *>::iterator iterColl = projOctObjects.begin(); iterColl != projOctObjects.end(); ++iterColl)
		{
			const char* tag = (*iterColl)->GetTag();//Get object's tag so we know how to handle collision

			//Destroy projectiles when they hit obstacles
			//This will ignore the floor... Might not be a problem though since they're destroyed after certain time anyways
			if (strcmp(tag,"Obstacle") == 0 && Collision::CheckCollision((*iterColl)->GetCollider(), (*projIter)->GetCollider()))
			{
				projectileManager.RemoveProjectileByAddress((*projIter));//Destroy projectile
			}
		}
	}
}

void GameManager::OnLeftClick()
{
	player.Shoot();
}

void GameManager::GameUpdate(float deltaTime)
{
	//1. Make sure game is has not ended
	if (!isGameOver()) {

		//Update the player
		player.Update(deltaTime);

		//Update all projectiles
		projectileManager.UpdateProjectiles(deltaTime);

		//Get and update enemies
		for (int i = 0; i < enemies.size(); i++)
		{
			enemies[i]->Update(deltaTime);
		}

		//Update what nodes objects are stored in, delete unnecessary ones
		spacePartitionHead.UpdateAll();

		CheckObjectCollisions(deltaTime);
	}
	else {
		ImGui::OpenPopup("EndGame");
	}
}


void GameManager::GameDraw(Renderer* renderer)
{
	renderer->SetViewProjMatrix(player.GetViewMatrix(), player.GetProjectionMatrix());

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
	projectileManager.DrawProjectiles(renderer);

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
}

bool GameManager::isGameOver()
{
	if (getTimeLeft() <= 0)
		return true;
	return false;
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
}

int GameManager::GetGameScore()
{
	return score;
}

void GameManager::ClearObjects()
{
	for (int i = 0; i < enemies.size(); i++)
	{
		delete enemies[i];
	}

	enemies.clear();

	for (int i = 0; i < gameObjects.size(); i++)
	{
		delete gameObjects[i];
	}

	gameObjects.clear();
}
