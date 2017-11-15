#pragma once
#include "Particle.h"
#include "SimpleShader.h"
#include <d3d11.h>
#include <vector>

class Emitter
{
public:
	Emitter();
	Emitter(
		int numParticles,
		int emitRate,
		float particleLifetime,
		float startSize,
		float endSize,
		DirectX::XMFLOAT4 startColor,
		DirectX::XMFLOAT4 endColor,
		DirectX::XMFLOAT3 startVelocity,
		DirectX::XMFLOAT3 emitterPosition,
		DirectX::XMFLOAT3 emitterAcceleration,
		SimpleVertexShader* vShader,
		SimplePixelShader* pShader,
		ID3D11ShaderResourceView* texture,
		ID3D11Device* device);
	~Emitter();

	void Update(float deltaTime); //Emitters update function that handles updating particles and emitter

	void ParticleUpdate(float deltaTime, int index); //helper function to do the update for a single particle

	void SpawnParticle();	//will Spawn particle from emitter

	void CopyParticleDataToGPU(ID3D11DeviceContext* context);
	void CopyToParticleVertex(int particleIndex);
	void Render(ID3D11DeviceContext* context, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj);	//renders all particles in emitter

private:
	//Emitter properties
	DirectX::XMFLOAT3 intialVelocity;	//velocity to emit particles at
	DirectX::XMFLOAT3 emitterParticleAcceleration;	//acceleration to apply to particles
	DirectX::XMFLOAT3 emitterPosition;	//position to emit particles from

	//Emission variables
	int maxParticles = 40; //Max particles that will be emitted by emitter. Defaults to 40
	int emmissionRate = 1; //Particles emitted per second
	float secondsPerParticle; //how long it takes to emit particle
	float timeSinceEmission; //used to tell how long since emitter emitted a particle
	bool loopable = false; //Should particles be recyled once they die
	bool active = false; //is the emitter currently on and emmiting particles

	//Rendering Variables
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ID3D11ShaderResourceView* particleTexture; //Texture that emitter particle will use
	SimpleVertexShader* vertexShader; //vertex Shader for particles to use
	SimplePixelShader* pixelShader;	//pixel Shader for particles to use

	//Particles Array Info
	Particle* particles;	//holds array of CPU side Particle Info for engine
	ParticleVertex* particleVertices; //array of particle vertices for particles in particles arrary. This data will be sent to GPU
	int firstAliveParticle;	//index to the current oldest living particle that is not dead 
	int firstDeadParticle;	//index to the current longest dead particle next to be emitted
	int aliveParticleCount; //# of particles that are alive

	//Particle Properties
	DirectX::XMFLOAT4 startColor;	//color particle starts as
	DirectX::XMFLOAT4 endColor;		//color particle should end as
	float startSize;	//size particle starts at
	float endSize;		//size particle should end at
	float particleLifetime; //how long a particle should be alive for
};

