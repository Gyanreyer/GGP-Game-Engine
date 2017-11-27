//Particle effects

struct VertexToPixel 
{
	float4 position	:SV_POSITION;
	float2 uv		:TEXCOORD0;
	float4 color	:TEXCOORD1;
};

//Particle Texture and Sampler
Texture2D particleTexture		:register(t0);
SamplerState trilinear			:register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	float4 particle = particleTexture.Sample(trilinear, input.uv) * input.color * input.color.a;
	return particle;
}