// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION; // XYZW position (System Value Position)
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 tangent		: TANGENT; //Tangent of the surface for normal maps, U direction
	float3 worldPos		: POSITION; //World position of this vertex
};

//DirectionalLight struct
struct DirectionalLight
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Direction;
};

//PointLight struct
struct PointLight
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Position;
};

//Constant buffer, pulls data from C++
cbuffer externalData : register(b0)
{
	DirectionalLight dLight1;
	DirectionalLight dLight2;
	PointLight pLight1;

	float3 cameraPosition; //For specular (reflection) calculation
};

//Texture variables
Texture2D diffuseTexture : register(t0);
Texture2D normalMap : register(t1);
SamplerState basicSampler : register(s0);

float4 calculateDirectionalLight(DirectionalLight light, float3 normal)
{
	return (light.DiffuseColor * saturate(dot(normal, normalize(-light.Direction)))) + light.AmbientColor;
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	//Normalize these, they may be larger due to interpolation
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	float4 textureColor = diffuseTexture.Sample(basicSampler,  input.uv);

	//Sample and unpack the normal
	float3 normalFromTexture = normalMap.Sample(basicSampler, input.uv).xyz * 2 - 1;
	
	//Create the Tangent-BiTangent-Normal (TBN) matrix
	//Translates from tangent to world space
	float3 N = input.normal;
	float3 T = normalize(input.tangent - N * dot(input.tangent, N));
	float3 B = cross(T, N);
	float3x3 TBN = float3x3(T, B, N);
	
	//Overwrite the existing normal with the one from
	//the normal map after it's been converted to world space
	input.normal = normalize(mul(normalFromTexture, TBN));

	return textureColor * (calculateDirectionalLight(dLight1, input.normal) + calculateDirectionalLight(dLight2, input.normal));
}