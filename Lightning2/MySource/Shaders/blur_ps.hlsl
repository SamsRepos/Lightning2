Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

#define MAX_MOTION_TEXTURES_NUM 4

#define TWO_PI 6.28318530718f

#define DEBUGOUTWHITE return float4(1.f, 1.f, 1.f, 1.f)

cbuffer ScreenSizeBuffer : register(b0)
{
	float2 screenSize;
	float2 padding;
};

cbuffer BlurParametersBuffer : register(b1)
{
	float directions;
	float quality;
	float size;
	float finalAdjustment;
}

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
	float2 radius = size / screenSize.xy;

	float4 colour = texture0.Sample(Sampler0, input.tex);
	
	float deltaTheta = TWO_PI / directions;
	for (float theta = 0.f; theta < TWO_PI; theta += deltaTheta)
	{
		float deltaI = 1.f / quality;
		for (float i = 1.f / quality; i < 1.f; i += deltaI)
		{
			float2 deltaTex = float2(cos(theta), sin(theta)) * radius * i;

			colour += texture0.Sample(				
				Sampler0,
				input.tex + deltaTex
			);
		}
	}

	colour /= quality * directions - finalAdjustment; //

	colour = saturate(colour);
	//colour.w = 1.f;

	return colour;
}