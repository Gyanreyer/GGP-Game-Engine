//Skybox

struct VertexToPixel
{
	float4 position : SV_POSITION;
	float3 uv		: TEXCOORD;
};

TextureCube skyboxTexture	: register(t0); //t0 = texture register 0
SamplerState skySampler		: register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	return skyboxTexture.Sample(skySampler, input. uv);
}