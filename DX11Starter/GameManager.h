#pragma once
#include "Enemy.h"
#include <time.h>
#include "ProjectileManager.h"
#include "Player.h"
#include "AssetManager.h"
#include <d3d11.h>

class GameManager
{
public:
	static GameManager& getInstance();
	~GameManager();

	//public game variables that anything in game can access
	double timeInMatch;

	void StartGame(AssetManager* asset, float screenWidth, float screenHeight, ID3D11DeviceContext* context);
	bool isGameOver();

	void AddScore(int addAmount);
	void ResetGame();

	//Game Get Methods
	Player* GetPlayer();
	ProjectileManager* GetProjectileManager();
	vector<Enemy>* GetEnemyVector();
	double getTimeLeft();
	int GetGameScore();

private:
	GameManager();
	//Stops compiler from generating methods to copy the object
	GameManager(GameManager const& copy);
	GameManager& operator=(GameManager const& copy);
	////////////////////////////////////////////////////////

	ProjectileManager projectileManager;

	time_t nowTime;
	tm gameStartTime;
	int score;

	//Array of Enemies so we can draw them in an easy loop
	//Not pointers, just do these directly
	vector<Enemy> enemies;
	Player player;
};

