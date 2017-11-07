#include "ProjectileManager.h"
#include "Lights.h"

ProjectileManager::ProjectileManager()
{
}

ProjectileManager::ProjectileManager(Mesh * projMesh, Material * playerProjMat, Material * enemyProjMat, ID3D11DeviceContext * ctx)
{
	projectileMesh = projMesh;
	playerProjectileMaterial = playerProjMat;
	enemyProjectileMaterial = enemyProjMat;
	context = ctx;

	projectileLifetime = 3.0f;
}

ProjectileManager::~ProjectileManager()
{
}

void ProjectileManager::SpawnPlayerProjectile(XMFLOAT3 startPt, XMFLOAT3 direction)
{
	//Add new player projectile
	//Only spawn new projectiles when there aren't too many on screen
	if (playerProjectiles.size() < 3)
	{
		playerProjectiles.push_back(
			Projectile(projectileMesh,
				playerProjectileMaterial,//Use player proj material
				startPt,//Point bullet will start from
				direction,//Rotation for bullet to move in
				5.0f));//Move at speed of 5 units/second
	}
}

void ProjectileManager::SpawnEnemyProjectile(XMFLOAT3 startPt, XMFLOAT3 direction)
{
	//Add new enemy projectile
	enemyProjectiles.push_back(
		Projectile(projectileMesh,
			enemyProjectileMaterial,//Use enemy proj material
			startPt,//Point buller will start from
			direction,//Rotation for bullet to move in
			3.0f));//Enemy bullets move slower?
}

//Iterate through all projectiles and move them (or delete them if lifetime has expired)
void ProjectileManager::UpdateProjectiles(float deltaTime)
{
	//Iterator to iterate over projectiles
	vector<Projectile>::iterator iter;

	//Loop through player and then enemy projs, remove any that have expired and update rest
	for (iter = playerProjectiles.begin(); iter != playerProjectiles.end();) {
		if (iter->GetTimeAlive() > projectileLifetime)
			iter = playerProjectiles.erase(iter);
		else {
			iter->Update(deltaTime);
			++iter;
		}
	}

	for (iter = enemyProjectiles.begin(); iter != enemyProjectiles.end();) {
		if (iter->GetTimeAlive() > projectileLifetime)
			iter = enemyProjectiles.erase(iter);
		else {
			iter->Update(deltaTime);
			++iter;
		}
	}
}

//Iterate through and draw all projectiles
void ProjectileManager::DrawProjectiles(Renderer* renderer)
{

	for (int i = 0; i < playerProjectiles.size(); i++)
		renderer->Render(&playerProjectiles[i]);

	for (int i = 0; i < enemyProjectiles.size(); i++)
		renderer->Render(&enemyProjectiles[i]);
}

void ProjectileManager::RemovePlayerProjectile(int i)
{
	playerProjectiles.erase(playerProjectiles.begin() + i);
}

void ProjectileManager::RemoveEnemyProjectile(int i)
{
	enemyProjectiles.erase(enemyProjectiles.begin() + i);
}

////Player projectiles colliding with the enemies
//bool ProjectileManager::CheckPlayerProjectileCollision(GameObject* other)
//{
//	for (byte i = 0; i < playerProjectiles.size(); i++)
//	{
//		
//	}
//}
//
////Enemy projectiles colliding with the player
//bool ProjectileManager::CheckEnemyProjectileCollision(GameObject* other)
//{
//	return false;
//}

//Get the player's projectiles
vector<Projectile> ProjectileManager::GetPlayerProjectiles()
{
	return playerProjectiles;
}

//Get the list of projectiles the enemy fires
vector<Projectile> ProjectileManager::GetEnemyProjectiles()
{
	return enemyProjectiles;
}
