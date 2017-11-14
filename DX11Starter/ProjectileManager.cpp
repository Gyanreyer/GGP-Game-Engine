#include "ProjectileManager.h"
#include "OctreeNode.h"
//#include "Lights.h"

ProjectileManager::ProjectileManager()
{
}

ProjectileManager::ProjectileManager(Mesh * projMesh, Material * playerProjMat, Material * enemyProjMat, ID3D11DeviceContext * ctx, OctreeNode * headNode)
{
	projectileMesh = projMesh;
	playerProjectileMaterial = playerProjMat;
	enemyProjectileMaterial = enemyProjMat;
	context = ctx;

	projectileLifetime = 3.0f;

	spacePartitionHead = headNode;
}

ProjectileManager::~ProjectileManager()
{
}

void ProjectileManager::SpawnPlayerProjectile(XMFLOAT3 startPt, XMFLOAT3 direction)
{
	//Add new player projectile
	Projectile newProj(projectileMesh,
		playerProjectileMaterial,//Use player proj material
		startPt,//Point bullet will start from
		direction,//Rotation for bullet to move in
		7.0f);//Move at speed of 7 units/second

	spacePartitionHead->AddObject(&newProj);//Add to spatial partition octree for coll management

	//Store new player projectile
	//playerProjectiles.push_back(newProj);
	projectiles.push_back(newProj);
}

void ProjectileManager::SpawnEnemyProjectile(XMFLOAT3 startPt, XMFLOAT3 direction)
{
	Projectile newProj(projectileMesh,
		enemyProjectileMaterial,//Use enemy proj material
		startPt,//Point buller will start from
		direction,//Rotation for bullet to move in
		5.0f);//Move at speed of 5 units/second, slower than player's bullets

	spacePartitionHead->AddObject(&newProj);//Add to spatial partition octree

	//Store new enemy projectile
	//enemyProjectiles.push_back(newProj);
	projectiles.push_back(newProj);
}

//Iterate through all projectiles and move them (or delete them if lifetime has expired)
void ProjectileManager::UpdateProjectiles(float deltaTime)
{
	//Loop through player and then enemy projs, remove any that have expired and update rest
	for (vector<Projectile>::iterator iter = projectiles.begin(); iter != projectiles.end();) {
		if (iter->GetTimeAlive() > projectileLifetime)
		{
			iter = RemoveProjectile(iter);
		}
		else
		{
			iter->Update(deltaTime);
			++iter;
		}
	}
}

//Iterate through and draw all projectiles
void ProjectileManager::DrawProjectiles(Renderer* renderer)
{
	for (int i = 0; i < projectiles.size(); i++)
		renderer->Render(&projectiles[i]);
}

void ProjectileManager::RemoveProjectile(int i)
{
	projectiles[i].GetOctNode()->RemoveObject(&projectiles[i]);//Stop tracking object in octree
	projectiles.erase(projectiles.begin() + i);//Erase from vector
}

vector<Projectile>::iterator ProjectileManager::RemoveProjectile(vector<Projectile>::iterator proj)
{
	proj->GetOctNode()->RemoveObject(&(*proj));//Stop tracking object in octree
	return projectiles.erase(proj);//Erase from vector and return new iterator
}

//Search for and remove a projectile given its address in memory
void ProjectileManager::RemoveProjectileByAddress(GameObject * proj)
{
	for (vector<Projectile>::iterator iter = projectiles.begin(); iter != projectiles.end();++iter)
	{
		if (&(*iter)==proj)
		{
			RemoveProjectile(iter);
			return;
		}
	}
}

vector<Projectile> ProjectileManager::GetProjectiles()
{
	return projectiles;
}