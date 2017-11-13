#pragma once
#include "Particle.h"
#include "SimpleShader.h"
#include <d3d11.h>
#include <vector>

class Emitter
{
public:
	Emitter();
	Emitter(int numParticles, int emitRate, float particleLifetime, float startSize, float endSize);
	~Emitter();

private:
	//Emission variables
	int maxParticles = 40; //Max particles that will be emitted by emitter. Defaults to 40
	int emmissionRate = 1; //Particles emitted per second
	bool loopable = false; //Should particles be recyled once they die
	bool active = false; //is the emitter currently on and emmiting particles

	//Rendering Variables
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ID3D11ShaderResourceView* particleTexture; //Texture that emitter particle will use
	SimpleVertexShader* vertexShader; //vertex Shader for particles to use
	SimplePixelShader* pixelShader;	//pixel Shader for particles to use

	//Particles Info
	Particle* particles;	//holds array of CPU side Particle Info for engine
	ParticleVertex* particleVertices; //array of particle vertices for particles in particles arrary. This data will be sent to GPU
	int firstAliveParticle;	//index to the current oldest living particle that is not dead 
	int firstDeadParticles;	//index to the current longest dead particle next to be emitted
};

