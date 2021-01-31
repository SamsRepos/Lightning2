Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

#define MAX_MOTION_TEXTURES_NUM 4

Texture2D motionTexture1 : register(t1);
Texture2D motionTexture2 : register(t2);
Texture2D motionTexture3 : register(t3);
Texture2D motionTexture4 : register(t4);

#define DEBUGOUTWHITE return float4(1.f, 1.f, 1.f, 1.f)

cbuffer ScreenSizeBuffer : register(b0)
{
	float2 screenSize;
	float2 padding;
};

cbuffer GaussianBuffer : register(b1)
{
	float gaussianExtent;
	float gaussianRange;
	float2 paddingForGaussian;
}

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

//////////////////////
//	  FUNCTIONS:	//
//////////////////////

float4 calculateGaussian(float2 inputTex, int extent, float range)
{
	
	if (range <= 1.f)
	{
		return texture0.Sample(Sampler0, inputTex);
	}

	float4 outColour = float4(0.f, 0.f, 0.f, 0.f);

	float deltaX = (1.0f / screenSize.x);
	float deltaY = (1.0f / screenSize.y);

	float weightsSum = 0.f;

	float weight = 1.f;

	//this pixel first:
	outColour += texture0.Sample(Sampler0, inputTex) * weight;

	for (int i = 0; i < (range * .5f); i++)
	{
		weight *= extent; // extent is used as a scaledown
		float2 sourceTex;

		//above
		sourceTex = inputTex;
		sourceTex.y -= deltaY * i;
		outColour += texture0.Sample(Sampler0, sourceTex) * weight;
		weightsSum += weight;

		//below
		sourceTex = inputTex;
		sourceTex.y += deltaY * i;
		outColour += texture0.Sample(Sampler0, sourceTex) * weight;
		weightsSum += weight;

		//left
		sourceTex = inputTex;
		sourceTex.x -= deltaX * i;
		outColour += texture0.Sample(Sampler0, sourceTex) * weight;
		weightsSum += weight;

		//right
		sourceTex = inputTex;
		sourceTex.x += deltaX * i;
		outColour += texture0.Sample(Sampler0, sourceTex) * weight;
		weightsSum += weight;
						
	}

	//outColour.a = 1.f;
	
	outColour.x = outColour.x / weightsSum;
	outColour.y = outColour.y / weightsSum;
	outColour.z = outColour.z / weightsSum;
	outColour.a = outColour.a / weightsSum;
	
	return outColour;
}

//////////////////////
//		MAIN:		//
//////////////////////

float4 main(InputType input) : SV_TARGET
{

	///float ddd = screenSize.x / 1000.f;
	///return float4(ddd, ddd, ddd, 1.f);

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	float4 textureColour = texture0.Sample(Sampler0, input.tex);

	float4 outputColour = float4(0.f, 0.f, 0.f, 0.f);

	//if doing directional:
	if (gaussianExtent > 0.f && gaussianRange > 0.f)	{
		outputColour += calculateGaussian(input.tex, gaussianExtent, gaussianRange);
	}
	
	outputColour = saturate(outputColour);
	//outputColour.w = 1.f;
	return outputColour;
}