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

void ProjectileManager::SpawnPlayerProjectile(XMFLOAT3 startPt, XMFLOAT3 rotation)
{
	//Add new player projectile
	playerProjectiles.push_back(
		Projectile(projectileMesh,
			playerProjectileMaterial,//Use player proj material
			ColliderType::SPHERE,//Use sphere collider
			context,
			startPt,//Point bullet will start from
			rotation,//Rotation for bullet to move in
			5.0f));//Move at speed of 5 units/second
}

void ProjectileManager::SpawnEnemyProjectile(XMFLOAT3 startPt, XMFLOAT3 rotation)
{
	//Add new enemy projectile
	enemyProjectiles.push_back(
		Projectile(projectileMesh,
			enemyProjectileMaterial,//Use enemy proj material
			ColliderType::SPHERE,//Use sphere collider
			context,
			startPt,//Point buller will start from
			rotation,//Rotation for bullet to move in
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

//Store shader data for projectile materials
void ProjectileManager::SetProjectileShaderData(std::string name, void * data, unsigned int size)
{
	playerProjectileMaterial->GetPixelShader()->SetData(name, data, size);
	enemyProjectileMaterial->GetPixelShader()->SetData(name, data, size);
}

//Iterate through and draw all projectiles
void ProjectileManager::DrawProjectiles(XMFLOAT4X4 viewMat, XMFLOAT4X4 projMat)
{
	vector<Projectile>::iterator iter;

	for (iter = playerProjectiles.begin(); iter != playerProjectiles.end(); ++iter)
		iter->Draw(viewMat, projMat);

	for (iter = enemyProjectiles.begin(); iter != enemyProjectiles.end(); ++iter)
		iter->Draw(viewMat, projMat);
}

void ProjectileManager::RemovePlayerProjectile(int i)
{
	playerProjectiles.erase(playerProjectiles.begin() + i);
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
