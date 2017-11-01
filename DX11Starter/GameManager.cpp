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

void GameManager::AddScore(int addAmount)
{
	score += addAmount;
}

void GameManager::ResetGame()
{
}

int GameManager::GetGameScore()
{
	return score;
}
