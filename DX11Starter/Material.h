#pragma once
#include "SimpleShader.h"

class Material
{
public:
	Material(SimpleVertexShader * vs, SimplePixelShader * ps, ID3D11ShaderResourceView * srv, ID3D11SamplerState * sampler);
	Material(SimpleVertexShader * vs, SimplePixelShader * ps, ID3D11ShaderResourceView * srv, ID3D11ShaderResourceView * nSRV, ID3D11SamplerState * sampler); //Material with normal
	~Material();

	SimpleVertexShader * GetVertexShader();
	SimplePixelShader * GetPixelShader();

	ID3D11ShaderResourceView * GetSRV();
	ID3D11ShaderResourceView * GetNormalSRV();
	ID3D11SamplerState * GetSampler();

private:
	SimpleVertexShader * vertexShader;
	SimplePixelShader * pixelShader;

	ID3D11ShaderResourceView * textureSRV;
	ID3D11ShaderResourceView * normalSRV;
	ID3D11SamplerState * textureSampler;
};

