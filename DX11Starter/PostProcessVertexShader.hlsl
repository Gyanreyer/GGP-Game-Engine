//Calculates post processing effects
//Bloom

//Output from this shader (Vertex shader) to the Pixel shader
struct VertexToPixel
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

//Entry point
VertexToPixel main(uint id : SV_VertexID)
{
	//Initialize the output
	VertexToPixel output;

	//Calculate the UV from (0, 0) to (2, 2) via the ID
	output.uv = float2((id << 1) & 2, id & 2); //Bit shift and bit mask

	//Adjust the output position based on the UV
	output.position = float4(output.uv, 0, 1);
	output.position.x = output.position.x * 2 - 1;
	output.position.y = output.position.y * -2 + 1;

	return output;
}