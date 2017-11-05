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
	timeInMatch = 15; //intializes how much time is in a game
	score = 0; //sets score to 0

	//PLAYER
	player = Player(BOX, (unsigned int)screenWidth, (unsigned int)screenHeight);

	//PROJECTILE MANAGER
	projectileManager = ProjectileManager(asset->GetMesh("Sphere"),
		asset->GetMaterial("HazardCrateMat"),//Placeholder until make new mats for bullets
		asset->GetMaterial("PurpleGhost"),
		context);

	CreateGameObjects(asset, context);
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
	enemies.push_back(Enemy(enemyTransform, asset->GetMesh("RustyPete"), asset->GetMaterial("RustyPeteMaterial"), BOX, true, context, 10, false, false, &projectileManager));
	enemyTransform.SetPosition(-2, 2, 0);
	enemies.push_back(Enemy(enemyTransform, asset->GetMesh("PurpleGhost"), asset->GetMaterial("PurpleGhost"), BOX, false, context, 20, false, true, &projectileManager));
	enemyTransform.SetPosition(0, 0, -2);
	enemies.push_back(Enemy(enemyTransform, asset->GetMesh("RustyPete"), asset->GetMaterial("RustyPeteMaterial"), BOX, true, context, 20, true, false, &projectileManager));

	///OTHER GAMEOBJECTS
	gameObjects.clear(); //Clear this out for new game instances

	//Store references to all GOs in vector
	gameObjects.push_back(GameObject(asset->GetMesh("Plane"), asset->GetMaterial("RustyPeteMaterial"), BOX, false, context));
	gameObjects.back().GetTransform()->SetScale(10, 0.001f, 10); //The floor is real small, for the sake of collisions
	gameObjects.push_back(GameObject(asset->GetMesh("Cube"), asset->GetMaterial("StoneMat"), BOX, false, context));
	gameObjects.back().GetTransform()->SetPosition(4, 0.5f, -2);
	gameObjects.push_back(GameObject(asset->GetMesh("Cube"), asset->GetMaterial("StoneMat"), BOX, false, context));
	gameObjects.back().GetTransform()->SetScale(0.5f, 0.5f, 0.5f);
	gameObjects.back().GetTransform()->SetPosition(2, 0.25f, -2);
	gameObjects.push_back(GameObject(asset->GetMesh("Sphere"), asset->GetMaterial("StoneMat"), SPHERE, false, context));
	gameObjects.back().GetTransform()->SetScale(0.5f, 0.5f, 0.5f);
	gameObjects.back().GetTransform()->SetPosition(-2, 0.25f, -2);
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
		}
	}
	else {
		ImGui::OpenPopup("EndGame");
	}
}


void GameManager::GameDraw(Renderer* renderer)
{
	//XMFLOAT4X4 viewMat = player->GetViewMatrix();
	//XMFLOAT4X4 projMat = player->GetProjectionMatrix();

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
	char intChar[10];
	score += itoa(GetGameScore() ,intChar, 10);
	std::string health = "Health: ";
	health += itoa(player.GetHealth(), intChar, 10);
	std::string timeLeft = "Time Left: ";
	timeLeft += itoa((int)getTimeLeft(), intChar, 10);
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

//ProjectileManager * GameManager::GetProjectileManager()
//{
//	return &projectileManager;
//}

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

