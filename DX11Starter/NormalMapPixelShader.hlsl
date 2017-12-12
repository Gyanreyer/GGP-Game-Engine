//Base pixel shader with normal mapping
//Calculates texture information
//Calculates normal map information
//Calculates directional and point lights
//Fog
//Shadow mapping

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
	float4 shadowMapPosition : POSITION1;
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
	PointLight pLight1;
	PointLight pLight2;

	float4 ambientLight; //The amount of ambient light in the scene
	float3 cameraPosition; //For specular (reflection) calculation
};

//Texture variables
Texture2D diffuseTexture : register(t0);
Texture2D normalMap : register(t1);
Texture2D ShadowMap		 : register(t2);
SamplerState basicSampler : register(s0);
SamplerComparisonState ShadowSampler : register(s1);

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

	const float minDist = 3; //Min dist before fog is applied
	const float maxDist = 15; //Max dist before fog fully obscures object

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
	input.tangent = normalize(input.tangent);

	float4 textureColor = diffuseTexture.Sample(basicSampler, input.uv);

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

	//Shadows: Calculate how "in shadow" this Pixel is
	float2 shadowUV = input.shadowMapPosition.xy / input.shadowMapPosition.w * 0.5f + 0.5f;
	shadowUV.y = 1.0f - shadowUV.y; //Flip the Y's

	//Calculate this pixel's actual depth from the light
	float depthFromLight = input.shadowMapPosition.z / input.shadowMapPosition.w;

	//sample the shadow map
	float shadowAmount = ShadowMap.SampleCmpLevelZero(
		ShadowSampler,		// Special "comparison" sampler
		shadowUV,			//where in the shadow map to look
		depthFromLight);	//The depth to compare againist

	float4 finalColor = textureColor *
		(ambientLight + //Ambient light in the scene
			calculateDirectionalLight(dLight1, input.normal) * shadowAmount + //Directional lights
			calculateLambertPointLight(pLight1, input) + calculateLambertPointLight(pLight2, input) //Point lights (These have a flat lighting effect)
			//calculateBlinnPhongPointLight(pLight1, input, cameraPosition) + calculateBlinnPhongPointLight(pLight2, input, cameraPosition) (This would make the object look wet)
			);

	//Lerp final color with fog factor to apply fog
	return lerp(finalColor, float4(0.1f, 0.1f, 0.1f, 1), getFogFactor(input.worldPos, cameraPosition));
}