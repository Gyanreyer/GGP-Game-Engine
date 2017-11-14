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
}

void GameManager::StartGame(AssetManager * asset, float screenWidth, float screenHeight, ID3D11DeviceContext* context)
{
	time(&nowTime); //gets current time when game is launched
	gameStartTime = *localtime(&nowTime); //assigns that time to gameStartTime to keep track of the time when game first started
	timeInMatch = 99; //intializes how much time is in a game
	score = 0; //sets score to 0

	//PLAYER
	player = Player(
		Transform(XMFLOAT3(0,1,-5),//Init position
			XMFLOAT3(0,0,0),//Init rot
			XMFLOAT3(0.5f,1,0.5f)),//Init scale
		(unsigned int)screenWidth, (unsigned int)screenHeight);//Screen dimensions for projection matrix

	CreateGameObjects(asset, context);
	InitSpatialPartition();

	//PROJECTILE MANAGER
	projectileManager = ProjectileManager(asset->GetMesh("Sphere"),
		asset->GetMaterial("HazardCrateMat"), //Placeholder until make new mats for bullets
		asset->GetMaterial("PurpleGhost"),
		context,
		&spacePartitionHead);
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
	enemies.push_back(Enemy(enemyTransform, asset->GetMesh("RustyPete"), asset->GetMaterial("RustyPeteMaterial"), &projectileManager, 10, EnemyType::moveX));
	enemyTransform.SetPosition(-2, 2, 0);
	enemies.push_back(Enemy(enemyTransform, asset->GetMesh("PurpleGhost"), asset->GetMaterial("PurpleGhost"), &projectileManager, 20, EnemyType::moveY));
	enemyTransform.SetPosition(0, 0, -2);
	enemies.push_back(Enemy(enemyTransform, asset->GetMesh("RustyPete"), asset->GetMaterial("RustyPeteMaterial"), &projectileManager, 20, EnemyType::moveX));
	enemyTransform.SetPosition(0, 1, 0);
	enemies.push_back(Enemy(enemyTransform, asset->GetMesh("SphereHP"), asset->GetMaterial("RockMaterial"), &projectileManager, 20, EnemyType::moveX)); //DIRTY BUBBLE!

	///OTHER GAMEOBJECTS
	gameObjects.clear(); //Clear this out for new game instances

	//Store references to all GOs in vector
	gameObjects.push_back(GameObject(Transform(XMFLOAT3(0,0,0),XMFLOAT3(0,0,0),XMFLOAT3(10, 0.001f, 10)),
		asset->GetMesh("Plane"), asset->GetMaterial("RustyPeteMaterial"),"Floor"));
	gameObjects.push_back(GameObject(Transform(XMFLOAT3(4,0.5f,-2),XMFLOAT3(0,0,0),XMFLOAT3(1,1,1)),
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
	//NOTE TO SELF: ADD A TAG SYSTEM TO GAMEOBJECTS
	//ALSO GIVE OBJECTS A REFERENCE TO WHAT NODE THEY'RE IN
	spacePartitionHead = OctreeNode(XMFLOAT3(0,-20,0),500,nullptr);//Will have to discuss size of play area, for now 1000x1000

	for (int i = 0; i < gameObjects.size(); i++)
	{
		spacePartitionHead.AddObject(&gameObjects[i]);
	}

	for (int i = 0; i < enemies.size(); i++)
	{
		spacePartitionHead.AddObject(&enemies[i]);
	}

	spacePartitionHead.AddObject(&player);
}

void GameManager::CheckObjectCollisions()
{
	//Check collisions for player
	vector<GameObject *> playerOctObjects = player.GetOctNode()->GetAllContainedObjects();
	
	for (vector<GameObject *>::iterator iterColl = playerOctObjects.begin(); iterColl != playerOctObjects.end();++iterColl)
	{
		const char* tag = (*iterColl)->GetTag();//Get type of object as string

		if (strcmp(tag, "Player") == 0) continue;//Skip the player, no need to check against self

		//If projectile, check for collision and take damage if hit
		else if (strcmp(tag, "Projectile") == 0 && Collision::CheckCollisionSphereBox((*iterColl)->GetCollider(), player.GetCollider()))
		{
			player.DecrementHealth();
			projectileManager.RemoveProjectileByAddress(*iterColl);//Destroy projectile
		}
		//If an object or enemy, the player should collide and be unable to pass through them
		else if ((strcmp(tag, "Obstacle") == 0 || strcmp(tag, "Enemy") == 0 ) && Collision::CheckCollision((*iterColl)->GetCollider(), player.GetCollider()))
		{
			//Resolve collision
			//Will this cause issues with the floor? Hmmm
		}	
	}

	//Check collisions between enemies and projectiles
	for (vector<Enemy>::iterator enemyIter = enemies.begin(); enemyIter != enemies.end(); ++enemyIter)
	{
		vector<GameObject *> enemyOctObjects = enemyIter->GetOctNode()->GetAllContainedObjects();

		for (vector<GameObject *>::iterator iterColl = enemyOctObjects.begin(); iterColl != enemyOctObjects.end(); ++iterColl)
		{
			//If projectile, check for collision and die if hit
			if (strcmp((*iterColl)->GetTag(), "Projectile") == 0 && Collision::CheckCollision((*iterColl)->GetCollider(), enemyIter->GetCollider()))
			{
				AddScore(enemyIter->GetPoints());//Add points to score
				enemyIter->GetOctNode()->RemoveObject(&(*enemyIter));//Remove enemy from octree
				enemyIter = enemies.erase(enemyIter) - 1;//Destroy enemy
				projectileManager.RemoveProjectileByAddress(*iterColl);//Destroy projectile
				break;
			}
		}
	}

	//Check collisions for projectiles with objects
	vector<Projectile> projs = projectileManager.GetProjectiles();

	for (vector<Projectile>::iterator projIter = projs.begin(); projIter != projs.end(); ++projIter)
	{
		vector<GameObject *> projOctObjects = projIter->GetOctNode()->GetAllContainedObjects();

		for (vector<GameObject *>::iterator iterColl = projOctObjects.begin(); iterColl != projOctObjects.end(); ++iterColl)
		{
			if (Collision::CheckCollision((*iterColl)->GetCollider(), projIter->GetCollider()))
			{
				projectileManager.RemoveProjectileByAddress(*iterColl);//Destroy projectile
			}
		}
	}


	/*
	//PLAYER PROJECTILE COLLISIONS
	for (byte i = 0; i < projectileManager.GetPlayerProjectiles().size(); i++)
	{
		Collider projCollider = *projectileManager.GetPlayerProjectiles()[i].GetCollider(); //The projectile's collider

																							//WITH ENEMIES
		for (byte j = 0; j < enemies.size(); j++)
		{
			if (Collision::CheckCollisionSphereBox(&projCollider, enemies[j].GetCollider()))
			{
				//Add score to player score
				AddScore(enemies[j].GetPoints());
				enemies.erase(enemies.begin() + j);
				projectileManager.RemovePlayerProjectile(i);
				goto break1; //Get out of the loop to prevent vector subscript errors
			}
		}

		//WITH OTHER GAMEOBJECTS
		for (byte j = 0; j < gameObjects.size(); j++)
		{
			Collider* goCollider = (gameObjects)[j].GetCollider(); //The GameObject's collider

			if (goCollider->collType == BOX && Collision::CheckCollisionSphereBox(&projCollider, goCollider))
			{
				projectileManager.RemovePlayerProjectile(i); //Simply delete projectile
				goto break1; //Get out of the loop to prevent vector subscript errors
			}
			else if (goCollider->collType == SPHERE && Collision::CheckCollisionSphereSphere(&projCollider, goCollider))
			{
				projectileManager.RemovePlayerProjectile(i); //Simply delete projectile
				goto break1; //Get out of the loop to prevent vector subscript errors
			}
		}
	}

break1: //This is super useful and I'm sad I didn't know about it sooner
	{
		//ENEMY PROJECTILE COLLISIONS
		for (byte i = 0; i < projectileManager.GetEnemyProjectiles().size(); i++)
		{
			Collider projCollider = *projectileManager.GetEnemyProjectiles()[i].GetCollider(); //The projectile's collider

																							   //WITH OTHER GAMEOBJECTS
			for (byte j = 0; j < gameObjects.size(); j++)
			{
				Collider* goCollider = gameObjects[j].GetCollider(); //The GameObject's collider

				if (goCollider->collType == BOX && Collision::CheckCollisionSphereBox(&projCollider, goCollider))
				{
					projectileManager.RemoveEnemyProjectile(i); //Simply delete projectile
					goto break2; //Get out of the loop to prevent vector subscript errors
				}
				else if (goCollider->collType == SPHERE && Collision::CheckCollisionSphereSphere(&projCollider, goCollider))
				{
					projectileManager.RemoveEnemyProjectile(i); //Simply delete projectile
					goto break2; //Get out of the loop to prevent vector subscript errors
				}
			}

			//WITH PLAYER
			if (Collision::CheckCollisionSphereBox(&projCollider, player.GetCollider()))
			{
				player.DecrementHealth();
				projectileManager.RemoveEnemyProjectile(i); //Remove the enemy's projectile, prevents multi-frame collisions
				goto break2; //Get out of the loop to prevent vector subscript errors
			}
		}
	}

break2: //This is super useful and I'm sad I didn't know about it sooner
	{
		//These brackets totally aren't a janky workaround at all
	}*/
}

void GameManager::GameUpdate(float deltaTime)
{
	//1. Make sure game is has not ended
	if (!isGameOver()) {

		player.Update(deltaTime);

		projectileManager.UpdateProjectiles(deltaTime);

		//Get and update enemies
		for (int i = 0; i < enemies.size(); i++)
		{
			(enemies)[i].Update(deltaTime);
		}

		spacePartitionHead.Update();

		CheckObjectCollisions();

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
		renderer->Render(&gameObjects[i]);
	}

	//Loop through Enemies and draw them
	for (byte i = 0; i < enemies.size(); i++)
	{
		renderer->Render(&enemies[i]);
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

ProjectileManager * GameManager::GetProjectileManager()
{
	return &projectileManager;
}

//Return the vector of gameObjects
vector<GameObject>* GameManager::GetGameObjectVector()
{
	return &gameObjects;
}

vector<Enemy>* GameManager::GetEnemyVector()
{
	return &enemies;
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
	enemies.clear();
}

int GameManager::GetGameScore()
{
	return score;
}

