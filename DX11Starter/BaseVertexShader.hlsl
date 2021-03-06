//Base vertex shader
//Calculates texture information
//Calculates directional and point lights
//Sets up shadow mapping
//Can set up for normal mapping
//Can set up for instanced rendering

// Constant Buffer
// - Allows us to define a buffer of individual variables 
//    which will (eventually) hold data from our C++ code
// - All non-pipeline variables that get their values from 
//    our C++ code must be defined inside a Constant Buffer
// - The name of the cbuffer itself is unimportant
cbuffer externalData : register(b0)
{
	//matrix world;
	matrix view;
	matrix projection;

	matrix shadowView;
	matrix shadowProj;
};

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 position		: POSITION; //XYZ position
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 tangent		: TANGENT; //Unused, but keeps consistiency with the C++ side

	//Part of our vertex input will be coming from a separate buffer
	//which requires us to set up a very specific ID3D11InputLayout, 
	//which describes where to get each piece of vertex data.
	//
	//However, SimpleShader has no way of knowing which data should
	//come from which buffer by just looking at the shader, since 
	//the input struct here doesn't know/care where the data comes from.
	//
	//To mark individual data as being "per instance" rather than
	//"per vertex", I set up SimpleShader to check the semantic name, 
	//looking for any that end with "_PER_INSTANCE".
	//
	//This is NOT a built-in DirectX semantic or idea - it's something
	//I had to do so that SimpleShader could correctly determine
	//how I wanted to set up the input layout.
	matrix instanceWorld : WORLD_PER_INSTANCE;
};

// Struct representing the data we're sending down the pipeline
// - Should match our pixel shader's input (hence the name: Vertex to Pixel)
// - At a minimum, we need a piece of data defined tagged as SV_POSITION
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;	// XYZW position (System Value Position)
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 tangent		: TANGENT;
	float3 worldPos		: POSITION; //World position of this vertex
	float4 shadowMapPosition : POSITION1;
};

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel main(VertexShaderInput input)
{
	// Set up output struct
	VertexToPixel output;

	// The vertex's position (input.position) must be converted to world space,
	// then camera space (relative to our 3D camera), then to proper homogenous 
	// screen-space coordinates.  This is taken care of by our world, view and
	// projection matrices.  
	//
	// First we multiply them together to get a single matrix which represents
	// all of those transformations (world to view to projection space)
	matrix worldViewProj = mul(mul(input.instanceWorld, view), projection);

	// Then we convert our 3-component position vector to a 4-component vector
	// and multiply it by our final 4x4 matrix.
	//
	// The result is essentially the position (XY) of the vertex on our 2D 
	// screen and the distance (Z) from the camera (the "depth" of the pixel)
	output.position = mul(float4(input.position, 1.0f), worldViewProj);

	//Shadows: Calculate where this vertex ended up in the Shadow map itself
	matrix shadowWVP = mul(mul(input.instanceWorld, shadowView), shadowProj);
	output.shadowMapPosition = mul(float4(input.position, 1.0f), shadowWVP);

	//World position of this vertex
	//Used for point/spot lights
	output.worldPos = mul(float4(input.position, 1.0f), input.instanceWorld).xyz;

	//Normal has to be in world space, normalized
	output.normal = normalize(mul(input.normal, (float3x3)input.instanceWorld));

	//Tangent has to be in world space, normalized
	output.tangent = normalize(mul(input.tangent, (float3x3)input.instanceWorld));

	output.uv = input.uv; //Pass UV through

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}