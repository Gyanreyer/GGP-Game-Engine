#pragma once
#include "Projectile.h"
#include "Renderer.h"

//Refactor after context is moved
class Renderer;
class ProjectileManager
{
public:
	ProjectileManager();
	ProjectileManager(Mesh * projMesh, Material * playerProjMat, Material * enemyProjMat, ID3D11DeviceContext * ctx, OctreeNode * headNode);
	~ProjectileManager();

	//Spawn projectiles
	void SpawnPlayerProjectile(XMFLOAT3 startPt, XMFLOAT3 direction);
	void SpawnEnemyProjectile(XMFLOAT3 startPt, XMFLOAT3 direction);

	void UpdateProjectiles(float deltaTime);//Update all projectiles

	//Draw all projectiles
	void DrawProjectiles(Renderer* renderer);
	void RemoveProjectile(int i);
	vector<Projectile *>::iterator RemoveProjectile(vector<Projectile *>::iterator proj);
	void RemoveProjectileByAddress(GameObject * proj);

	vector<Projectile *> GetProjectiles();

private:
	ID3D11DeviceContext * context;
	Mesh * projectileMesh;
	Material * playerProjectileMaterial;
	Material * enemyProjectileMaterial;

	vector<Projectile *> projectiles;

	float projectileLifetime;

	OctreeNode * spacePartitionHead;
};

