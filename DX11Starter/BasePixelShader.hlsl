//Base pixel shader
//Calculates texture information
//Calculates directional and point lights
//Fog

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
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 worldPos		: POSITION; //World position of this vertex
};

//DirectionalLight struct
struct DirectionalLight
{
	float4 DiffuseColor;
	float3 Direction;
};

//PointLight struct
struct PointLight
{
	float4 DiffuseColor;
	float3 Position;
};

//Constant buffer, pulls data from C++
cbuffer externalData : register(b0)
{
	DirectionalLight dLight1;
	DirectionalLight dLight2;
	PointLight pLight1;

	float4 ambientLight; //The amount of ambient light in the scene
	float3 cameraPosition; //For specular (reflection) calculation
};

//Texture variables
Texture2D diffuseTexture : register(t0);
SamplerState basicSampler : register(s0);

//Directional light diffuse calculations
float4 calculateDirectionalLight(DirectionalLight light, float3 normal)
{
	return light.DiffuseColor * saturate(dot(normal, normalize(-light.Direction)));
}

//Point light diffuse calculations
//(Diffuse + reflection)
float4 calculateLambertPointLight(PointLight light, VertexToPixel input)
{
	float3 directionToPointLight = normalize(light.Position - input.worldPos); //Direction from this vertex to the light
	float pointLightAmount = saturate(dot(input.normal, directionToPointLight)); //Amount of light from this point light

	return (light.DiffuseColor * pointLightAmount); //Return point light (color * amount)
}

//Point light specular calculations
//Phong, slower than Blinn-Phong
//(Diffuse + reflection)
float4 calculatePhongPointLight(PointLight light, VertexToPixel input, float3 cameraPos)
{
	float3 directionToPointLight = normalize(light.Position - input.worldPos); //Direction from this vertex to the light
	float pointLightAmount = saturate(dot(input.normal, directionToPointLight)); //Amount of light from this point light (NdotL)
	float3 directionToCamera = normalize(cameraPos - input.worldPos); //Direction to the camera from the current pixel

	float3 reflection = reflect(-directionToPointLight, input.normal); //Reflection vector of the incoming light
	float pointLightSpecular = pow(saturate(dot(reflection, directionToCamera)), 64); //Calculate the specular highlight (1-64 for shininess)

	return (light.DiffuseColor * pointLightAmount) + pointLightSpecular; //Return point light with specular reflection (color * amount + specular)
}

//Point light specular calculations
//Blinn-Phong, faster than Phong
//(Diffuse + reflection)
float4 calculateBlinnPhongPointLight(PointLight light, VertexToPixel input, float3 cameraPos)
{
	float3 directionToPointLight = normalize(light.Position - input.worldPos); //Direction from this vertex to the light
	float pointLightAmount = saturate(dot(input.normal, directionToPointLight)); //Amount of light from this point light (NdotL)
	float3 directionToCamera = normalize(cameraPos - input.worldPos); //Direction to the camera from the current pixel

	float3 halfWayVector = normalize(directionToPointLight + directionToCamera); //Halfway vector between the surface and camera
	float NdotH = saturate(dot(input.normal, halfWayVector)); //Cosine of the normal and halfway vectors
	float pointLightSpecular = pow(NdotH, 64); //Calculate the specular highlight (1-64 for shininess)

	return (light.DiffuseColor * pointLightAmount) + pointLightSpecular; //Return point light with specular reflection (color * amount + specular)
}

//Calculate how strong the fog should be
float getFogFactor(float3 pos, float3 cameraPos)
{
	float dist = distance(pos, cameraPos); //Get dist from camera

	const float minDist = 5; //Min dist before fog is applied
	const float maxDist = 20; //Max dist before fog fully obscures object

	//Return how strong the fog should be as a percentage 0-1
	return saturate(1 - (maxDist - dist) / (maxDist - minDist));
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

	float4 textureColor = diffuseTexture.Sample(basicSampler, input.uv);

	float4 finalColor = textureColor *
		(ambientLight + //Ambient light in the scene
			calculateDirectionalLight(dLight1, input.normal) + calculateDirectionalLight(dLight2, input.normal) + //Directional lights
			calculateLambertPointLight(pLight1, input) //Point lights
			);

	//Lerp final color with fog factor to apply fog
	return lerp(finalColor, float4(0.2, 0.2, 0.2, 1), getFogFactor(input.worldPos, cameraPosition));
}