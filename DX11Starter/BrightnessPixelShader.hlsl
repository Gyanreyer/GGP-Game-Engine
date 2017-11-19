//Calculates bloom

//Output from the Vertex shader to this shader (Pixel shader)
struct VertexToPixel
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

//Textures and sampler state
Texture2D Pixels : register(t0);
SamplerState Sampler : register(s0);

//Entry point
float4 main(VertexToPixel input) : SV_TARGET
{
	//https://stackoverflow.com/questions/596216/formula-to-determine-brightness-of-rgb-color
	float4 brightPixelSample = Pixels.Sample(Sampler, input.uv); //Sample the pixel from the pixel map
	float brightPixelBrightness = dot(brightPixelSample.rgb, float3(0.299f, 0.587f, 0.114f)); //Get the dot product of the bright pixel sample

	//Determine if the pixel is bright enough for bloom
	if (brightPixelBrightness < .9f) //brightPixelBrightness is a 0-1 value
		return float4(0, 0, 0, 0);

	return brightPixelSample;
}