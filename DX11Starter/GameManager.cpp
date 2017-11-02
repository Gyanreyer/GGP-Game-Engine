#include "GameManager.h"



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
	enemies.clear();

	//Create an enemy
	enemies.push_back(Enemy(XMFLOAT3(2, 0, 0), asset->GetMesh("RustyPete"), asset->GetMaterial("RustyPeteMaterial"), BOX, true, context, 10, false, false));
	enemies.push_back(Enemy(XMFLOAT3(-2, 2, 0), asset->GetMesh("PurpleGhost"), asset->GetMaterial("PurpleGhost"), BOX, false, context, 20, false, true));
	enemies.push_back(Enemy(XMFLOAT3(0, 0, -2), asset->GetMesh("RustyPete"), asset->GetMaterial("RustyPeteMaterial"), BOX, true, context, 20, true, false));

	///OTHER GAMEOBJECTS
	gameObjects.clear();

	//Store references to all GOs in array
	gameObjects.push_back(GameObject(asset->GetMesh("Plane"), asset->GetMaterial("RustyPeteMaterial"), BOX, false, context));
	gameObjects.back().GetTransform()->SetScale(10, 0.001f, 10); //The floor is real small, for the sake of collisions
	gameObjects.push_back(GameObject(asset->GetMesh("Cube"), asset->GetMaterial("StoneMat"), BOX, false, context));
	gameObjects.back().GetTransform()->SetScale(0.5f, 0.5f, 0.5f);
	gameObjects.back().GetTransform()->SetPosition(2, 0.5f, -2);
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

