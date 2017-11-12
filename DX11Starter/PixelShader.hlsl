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

float4 getLightingColor(float3 normal, DirectionalLight light)
{
	return light.AmbientColor + (light.DiffuseColor * saturate(dot(normal,normalize(-light.Direction))));
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
	float4 surfaceColor = diffuseTexture.Sample(basicSampler,  input.uv);

	return surfaceColor *
		(getLightingColor(input.normal, dLight1) + getLightingColor(input.normal, dLight2));
}