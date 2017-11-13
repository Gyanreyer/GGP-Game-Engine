cbuffer externalData : register(b0)
{
	matrix view; 
	matrix projection;
};

struct VertexShaderInput 
{
	float3 position : POSITION;
	float2 uv		: TEXCOORD;
	float3 normal	: NORMAL;
};

struct VertexToPixel 
{
	float4 position : SV_POSITION;
	float3 uv		: TEXCOORD;
};

VertexToPixel main( VertexShaderInput input )
{
	VertexToPixel output;

	//calculate view matrix without translations
	matrix viewNoTranslations = view;
	viewNoTranslations._41 = 0;
	viewNoTranslations._42 = 0;
	viewNoTranslations._43 = 0;

	//calculate position
	matrix viewProj = mul(viewNoTranslations, projection);
	output.position = mul(float4(input.position, 1.0f), viewProj);

	output.position.z = output.position.w;

	output.uv = input.position;

	return output;
}