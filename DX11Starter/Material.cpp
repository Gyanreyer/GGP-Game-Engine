#include "Material.h"

//Material with normal
Material::Material(SimpleVertexShader * vs, SimplePixelShader * ps, ID3D11ShaderResourceView * srv, ID3D11SamplerState * sampler)
{
	vertexShader = vs;
	pixelShader = ps;

	textureSRV = srv;
	normalSRV = NULL;
	textureSampler = sampler;
}

//Material without normal
Material::Material(SimpleVertexShader * vs, SimplePixelShader * ps, ID3D11ShaderResourceView * srv, ID3D11ShaderResourceView * nSRV, ID3D11SamplerState * sampler)
{
	vertexShader = vs;
	pixelShader = ps;

	textureSRV = srv;
	normalSRV = nSRV;
	textureSampler = sampler;
}

Material::~Material()
{
}

SimpleVertexShader * Material::GetVertexShader()
{
	return vertexShader;
}

SimplePixelShader * Material::GetPixelShader()
{
	return pixelShader;
}

ID3D11ShaderResourceView * Material::GetSRV()
{
	return textureSRV;
}

ID3D11ShaderResourceView * Material::GetNormalSRV()
{
	return normalSRV;
}

ID3D11SamplerState * Material::GetSampler()
{
	return textureSampler;
}

