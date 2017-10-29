#pragma once
#include "Projectile.h"

class ProjectileManager
{
public:
	ProjectileManager();
	ProjectileManager(Mesh * projMesh, Material * playerProjMat, Material * enemyProjMat, ID3D11DeviceContext * ctx);
	~ProjectileManager();

	//Spawn projectiles
	void SpawnPlayerProjectile(XMFLOAT3 startPt, XMFLOAT3 rotation);
	void SpawnEnemyProjectile(XMFLOAT3 startPt, XMFLOAT3 rotation);

	void UpdateProjectiles(float deltaTime);//Update all projectiles

	//Set pixel shader data for projectile materials... Potentially placeholder?
	void SetProjectileShaderData(std::string name, void * data, unsigned int size);
	//Draw all projectiles
	void DrawProjectiles(XMFLOAT4X4 viewMat, XMFLOAT4X4 projMat);

	//Investigate spatial partitioning?
	//Not sure what current state of collision detection is so will leave this for now
	bool CheckPlayerProjectileCollision(GameObject other);
	bool CheckEnemyProjectileCollision(GameObject other);

private:
	ID3D11DeviceContext * context;
	Mesh * projectileMesh;
	Material * playerProjectileMaterial;
	Material * enemyProjectileMaterial;

	vector<Projectile> playerProjectiles;
	vector<Projectile> enemyProjectiles;

	float projectileLifetime;
};

