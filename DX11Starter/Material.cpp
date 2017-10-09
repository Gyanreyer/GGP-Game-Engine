#include "Material.h"


Material::Material(SimpleVertexShader * vs, SimplePixelShader * ps, ID3D11ShaderResourceView * srv, ID3D11SamplerState * sampler)
{
	vertexShader = vs;
	pixelShader = ps;

	textureSRV = srv;
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

ID3D11SamplerState * Material::GetSampler()
{
	return textureSampler;
}
