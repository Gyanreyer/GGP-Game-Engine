#pragma once
#include <DirectXMath.h>

//Particle Info used by Emitter and CPU
struct Particle {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT3 startVel;
	float size;
	float age;
};

//Particle Info used by GPU
struct ParticleVertex {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT4 color;
	float size;
};

