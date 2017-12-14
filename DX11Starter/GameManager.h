#pragma once
#include <d3d11.h>
#include <time.h>
#include <string> //Int conversion to string
#include "Enemy.h"
#include "ProjectileManager.h"
#include "Emitter.h"
#include "Player.h"
#include "AssetManager.h"
#include "Renderer.h"
#include "OctreeNode.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "CommonStates.h"

using std::to_string; //Alternative to atoi

enum GameState {
	start,
	playing,
	end
};

class GameManager
{
public:
	static GameManager& getInstance();
	~GameManager();

	//public game variables that anything in game can access
	double timeInMatch;

	void StartGame(AssetManager* asset, float screenWidth, float screenHeight, ID3D11DeviceContext* context, ID3D11Device* device);
	void CreateGameObjects(AssetManager * asset, ID3D11DeviceContext* context, ID3D11Device* device); //Initializes GameObjects
	void GameUpdate(float deltaTime, float totalTime, Renderer* renderer);
	void GameDraw(Renderer* renderer);

	void AddScore(int addAmount);
	void ResetGame(Renderer* renderer);

	bool IsGameOver();

	//Engine Get Methods
	Player* GetPlayer();//We shouldn't need this, at some point need to do some housekeeping on Engine.cpp
	double getTimeLeft();
	double getTime();
	int GetGameScore();

	void OnLeftClick();

	GameState state;

private:
	GameManager();
	//Stops compiler from generating methods to copy the object
	GameManager(GameManager const& copy);
	GameManager& operator=(GameManager const& copy);
	////////////////////////////////////////////////////////

	ProjectileManager* projectileManager;

	time_t nowTime;
	tm gameStartTime;
	int score;

	//Vector of GameObjects so we can draw them in an easy loop
	vector<GameObject *> gameObjects;

	//Vectors for instanced GameObjects
	//Each different mesh needs it's own vector for instancing
	vector<GameObject *> trees;
	vector<GameObject *> lamps;
	vector<GameObject *> barrels;

	//Vector of Enemies so we can draw them in an easy loop
	vector<Enemy *> enemies;
	Emitter* campfireEmitter;
	Emitter* smokeEmitter;
	Player player;

	OctreeNode* spacePartitionHead;

	void InitSpatialPartition();
	void CheckObjectCollisions(float deltaTime, float totalTime);

	void ClearObjects();
	void UpdateObjectsForRenderer(Renderer* renderer);
};

