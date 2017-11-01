#pragma once
#include "Enemy.h"

class GameManager
{
public:
	static GameManager& getInstance();
	~GameManager();
	void UpdateEnemies();
	void AddScore(int addAmount);
	void ResetGame();
	int GetGameScore();

private:
	GameManager();
	//Stops compiler from generating methods to copy the object
	GameManager(GameManager const& copy);
	GameManager& operator=(GameManager const& copy);
	////////////////////////////////////////////////////////
	int score;
	vector<Enemy*> enemies;

};

