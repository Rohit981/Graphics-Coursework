SamplerState ColorMapSampler : register(s0);
SamplerState UserMapSampler : register(s1);
Texture2D shaderTexture : register(t0);
Texture2D shaderTex : register(t1);

cbuffer GodRayBuffer : register(b0)
{
	float4 lightPosition;
	float2 screeSize;
	float2 padding;
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos : TEXCOORD1;

};

float4 main(InputType input) : SV_TARGET
{
	
	float4 lightPositionOnScreen = lightPosition;
	lightPositionOnScreen = mul(lightPositionOnScreen, viewMatrix);
	lightPositionOnScreen = mul(lightPositionOnScreen, projectionMatrix);


	lightPositionOnScreen.xyz /= lightPositionOnScreen.w;
	lightPositionOnScreen.xy *= float2(0.5f, -0.5f);
	lightPositionOnScreen.xy += 0.5f;
	
	float decay = 0.96815;
	float exposure = 0.2;
	float density = 0.926;
	float weight = 0.58767;

	/// NUM_SAMPLES will describe the rays quality, you can play with
	int NUM_SAMPLES = 100;


	float2 tc = input.tex;
	float2 deltaTexCoord = (tc - lightPositionOnScreen.xy);
	deltaTexCoord *= 1.0 / float(NUM_SAMPLES) * density;

	float illuminationDecay = 1.0;

	float4 textureColour = shaderTexture.Sample(UserMapSampler, tc) * 0.4f;

	 for (int i = 0; i < NUM_SAMPLES; i++) {

		tc -= deltaTexCoord;
		float4 smapleColour = shaderTexture.Sample(UserMapSampler, tc) * 0.4f;
		smapleColour *= illuminationDecay * weight;
		textureColour += smapleColour;
		illuminationDecay *= decay;

	 }

	 float4 realColor = shaderTex.Sample(ColorMapSampler, input.tex);

	 return ((float4((float3(textureColour.r, textureColour.g, textureColour.b) * exposure),1)) + (realColor * (1.1)));
}