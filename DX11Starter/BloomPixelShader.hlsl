//Calculates bloom

//Output from the Vertex shader to this shader (Pixel shader)
struct VertexToPixel
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

//Constant buffer, pulls data from C++
cbuffer Data : register(b0)
{
	float pixelWidth;
	float pixelHeight;
	int blurAmount;
}

//Textures and sampler state
Texture2D Pixels : register(t0);
Texture2D Render : register(t1); //Need this extra map for bloom
SamplerState Sampler : register(s0);

//Entry point
float4 main(VertexToPixel input) : SV_TARGET
{
	//Initialize variables for return later
	float4 totalColor = float4(0, 0, 0, 0);
	uint numSamples = 0;

	//Loop to blur pixels
	for (int x = -blurAmount; x <= blurAmount; x++)
	{
		for (int y = -blurAmount; y <= blurAmount; y++)
		{
			//Calculate the UV coordinate for a neighboring pixel
			float2 neighborUV = input.uv + float2(x * pixelWidth, y * pixelHeight);

			//Sample the neighboring pixel
			totalColor += Pixels.Sample(Sampler, neighborUV);

			numSamples++;
		}
	}

	return saturate(Render.Sample(Sampler, input.uv) + (totalColor / numSamples)); //Add the render frame to the blurred bright frame

	//return Render.Sample(Sampler, input.uv) + Pixels.Sample(Sampler, input.uv);
}