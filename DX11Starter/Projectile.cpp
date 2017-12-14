#include "Projectile.h"
#include "AssetManager.h"

Projectile::Projectile(Mesh * mesh, Material * material, ID3D11Device* device,
	XMFLOAT3 startPos, XMFLOAT3 direction, float speed) :
	GameObject(Transform(startPos,XMFLOAT3(0,0,0),XMFLOAT3(0.02f,0.02f,0.02f)),mesh, material, "Projectile")
{
	timeAlive = 0;
	moveSpeed = speed;//Store speed that will move by each frame
	
	//Make projectile face same direction as player
	transform.SetForward(direction);
	transform.MoveRelative(0.5f,0,0);

	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	//XMVECTOR particleVelocityV = -XMLoadFloat3(&direction);
	////particleVelocityV = -XMVector3Normalize(particleVelocityV);
	//XMFLOAT3 particleVelocity;
	//XMStoreFloat3(&particleVelocity, particleVelocityV);
	//
	//XMFLOAT3 particleAccel;
	//particleVelocityV = particleVelocityV * 0.03;
	//XMStoreFloat3(&particleAccel, particleVelocityV);

	AssetManager* asset = &AssetManager::getInstance();

	//Create Particle trail
	trail = new Emitter(
		200, //Number of Particles
		15,	 //Particle Emission Rate
		2.0f, //Particle Lifetime
		0.01f, //Start Size
		0.5f, //End Size
		XMFLOAT4(r, g, b, 1.0f), //Start Color
		XMFLOAT4(r, g, b, 0.0f), //End Color
		XMFLOAT3(-direction.x, -direction.y, -direction.z),
		startPos,
		XMFLOAT3(-direction.x, -direction.y, -direction.z),
		asset->GetVShader("ParticleShader"),
		asset->GetPShader("ParticleShader"),
		asset->GetTexture("TrailParticle"),
		device);
}

Projectile::~Projectile()
{
	delete trail;
}

//Move forward until out of range
void Projectile::Update(float deltaTime)
{
	//Move forward endlessly every frame
	transform.MoveRelative(moveSpeed*deltaTime,0,0);
	trail->SetPosition(transform.GetPosition());
	trail->Update(deltaTime);
	timeAlive += deltaTime;
}

float Projectile::GetTimeAlive()
{
	return timeAlive;
}

Emitter * Projectile::GetEmitter()
{
	return trail;
}
