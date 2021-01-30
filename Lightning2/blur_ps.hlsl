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

cbuffer DirectionalBuffer : register(b1)
{
	float directionalExtent;
	float directionalTheta;
	float2 paddingfordirecitonal;
}

cbuffer MotionBuffer : register(b2)
{
	float motionExtent;
	float3 paddingForMotion;
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

float4 calculateDirectional(float2 inputTex, int extent, float theta) {
	
	if (extent <= 1.f)
	{
		return texture0.Sample(Sampler0, inputTex);
	}

	float weight = 1.f / extent;

	float4 outColour = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float deltaX = (1.0f / screenSize.x) * cos(radians(theta));
	float deltaY = (1.0f / screenSize.y) * sin(radians(theta));

	float2 sourceTex = inputTex;

	for (int i = 0; i < extent; i++) {

		outColour += texture0.Sample(Sampler0, sourceTex) * weight;

		sourceTex.x += deltaX;
		sourceTex.y += deltaY;
	}

	outColour.a = 1.f;
	
	return outColour;
}

float4 calculateMotion(int extent, float2 inputTex) {
	
	if (extent > MAX_MOTION_TEXTURES_NUM
		|| extent < 1) {
		return texture0.Sample(Sampler0, inputTex);
	}
	
	float weight = 1.f / float(extent + 1);

	//setting the weighted current image's colour value first:
	float4 outColour = texture0.Sample(Sampler0, inputTex) * weight;
	
	//adding all other previous blur captures:
	outColour += motionTexture1.Sample(Sampler0, inputTex) * weight;
	--extent;
	if (extent <= 0) return outColour;
	
	outColour += motionTexture2.Sample(Sampler0, inputTex) * weight;
	--extent;
	if (extent <= 0) return outColour;

	outColour += motionTexture3.Sample(Sampler0, inputTex) * weight;
	--extent;
	if (extent <= 0) return outColour;
		
	outColour += motionTexture4.Sample(Sampler0, inputTex) * weight;
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

	float4 outputColour = float4(0.f, 0.f, 0.f, 1.f);

	float blursApplied = 0;

	//if doing directional:
	if (directionalExtent > 0.f)	{
		outputColour += calculateDirectional(input.tex, directionalExtent, directionalTheta);
		blursApplied++;
	}
	//if doing motion:
	if (motionExtent > 0.f) {
		outputColour += calculateMotion(motionExtent, input.tex);
		blursApplied++;
	}
		
	if (blursApplied == 0.f) return textureColour;
	outputColour /= blursApplied;
	outputColour = saturate(outputColour);
	outputColour.w = 1.f;
	return outputColour;
}