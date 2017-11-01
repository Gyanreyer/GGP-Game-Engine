#pragma once
#include "Enemy.h"
#include <time.h>

class GameManager
{
public:
	static GameManager& getInstance();
	~GameManager();

	double timeInMatch;

	void StartGame();
	bool isGameOver();
	double getTimeLeft();
	void AddScore(int addAmount);
	void ResetGame();
	int GetGameScore();

private:
	GameManager();
	//Stops compiler from generating methods to copy the object
	GameManager(GameManager const& copy);
	GameManager& operator=(GameManager const& copy);
	////////////////////////////////////////////////////////
	time_t nowTime;
	tm gameStartTime;
	int score;
	vector<Enemy*> enemies;

};

