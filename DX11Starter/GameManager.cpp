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
	timeInMatch = 80; //intializes how much time is in a game
	score = 0; //sets score to 0

	//Initialize Player and Projectile Manager
	player = Player(BOX, screenWidth, screenHeight);
	projectileManager = ProjectileManager(asset->GetMesh("Sphere"),
		asset->GetMaterial("HazardCrateMat"),//Placeholder until make new mats for bullets
		asset->GetMaterial("PurpleGhost"),
		context);

	//Create Enemies
	///ENEMIES
	//Create an enemy
	enemies.push_back(Enemy(XMFLOAT3(2, 0, 0), asset->GetMesh("RustyPete"), asset->GetMaterial("RustyPeteMaterial"), BOX, true, context, 10, false, false));

	//"Another one"
	enemies.push_back(Enemy(XMFLOAT3(-2, 2, 0), asset->GetMesh("PurpleGhost"), asset->GetMaterial("PurpleGhost"), BOX, false, context, 20, false, true));

	enemies.push_back(Enemy(XMFLOAT3(0, 0, -2), asset->GetMesh("RustyPete"), asset->GetMaterial("RustyPeteMaterial"), BOX, true, context, 20, true, false));
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
}

int GameManager::GetGameScore()
{
	return score;
}

