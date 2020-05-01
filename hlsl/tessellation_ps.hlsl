// Tessellation pixel shader
// Output colour passed to stage.

Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture : register(t1);
Texture2D depthMapTexture2 : register(t2);

SamplerState diffuseSampler  : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer LightBuffer : register(b0)
{
	float4 ambient;
	float4 diffuse;
	float4 direction;
};

cbuffer SpotLightBuffer : register(b1)
{
	float4 diffuseColour;
	float4 ambientColor;	
	float3 position;
	float specularPower;	
	float3 spotdirection;
	float angle;
	float4 specularColour;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal: NORMAL;
	float4 lightViewPos : TEXCOORD1;
	float4 lightViewPos2 : TEXCOORD2;
	float3 worldPosition: TEXCOORD3;
};


// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
	float intensity = saturate(dot(normal, lightDirection));
	float4 colour = saturate(diffuse * intensity);
	return colour;
}

float4 calcSpecular(float3 lightDirection, float3 normal, float3 viewVector, float4 specularColour, float4 specularPower)	
{
	float3 halfway = normalize(lightDirection + viewVector);
	float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);
	return saturate(specularColour * specularIntensity);
}

/// PCF calculation
float3 Offset_lookup(SamplerState map, float4 loc, float2 offset)
{
		int shadowmapWidth = 2024.f;
		int shadowmapHeight = 2024.f;

		float2 texmapscale;

		texmapscale.x  = 1/ shadowmapWidth;
		texmapscale.y  = 1/ shadowmapHeight;
		
		return depthMapTexture.SampleLevel(map, float4(loc.xy + offset * texmapscale * loc.w, loc.z, loc.w), 1);
}

// Calculate lighting intensity based on direction and normal. Combine with light colour.



float4 main(InputType input) : SV_TARGET
{
	float depthValue[1];
	float lightDepthValue[1];
	float shadowMapBias = 0.005f;
	/*float4 colour[2];
	colour[0] = float4(0, 0, 0, 1.f);
	colour[1] = float4(0, 0, 0, 1.f);*/
	float4 colour = float4(0.f, 0.f, 0.f, 1.f);
	float4 textureColour = shaderTexture.Sample(diffuseSampler, input.tex);

	
	// Calculate the projected texture coordinates.
	float2 pTexCoord = input.lightViewPos.xy / input.lightViewPos.w;
	pTexCoord *= float2(0.5, -0.5);
	pTexCoord += float2(0.5f, 0.5f);

	// Determine if the projected coordinates are in the 0 to 1 range.  If not don't do lighting.
	if (pTexCoord.x < 0.f || pTexCoord.x > 1.f || pTexCoord.y < 0.f || pTexCoord.y > 1.f)
	{
		return textureColour;
	}

	//float2 pTexCoord2 = input.lightViewPos.xy / input.lightViewPos.w;
	//pTexCoord2 *= float2(0.5, -0.5);
	//pTexCoord2 += float2(0.5f, 0.5f);

	//// Determine if the projected coordinates are in the 0 to 1 range.  If not don't do lighting.
	//if (pTexCoord2.x < 0.f || pTexCoord2.x > 1.f || pTexCoord2.y < 0.f || pTexCoord2.y > 1.f)
	//{
	//	return textureColour;
	//}

	float sum = 0;
	float x, y;

	for (y = -3; y <= 3; y += 1.0)
		for (x = -3; x <=3; x += 1.0)
			sum += Offset_lookup(shadowSampler,float4( pTexCoord,1,1), float2(x, y));

	///setting the shadow map equal to the sum bias in order to have soft shadows 
	shadowMapBias = sum / 4.0;

	// Sample the shadow map (get depth of geometry)
	depthValue[0] = depthMapTexture.Sample(shadowSampler, pTexCoord).r;

	// Calculate the depth from the light.
	lightDepthValue[0] = input.lightViewPos.z / input.lightViewPos.w;
	lightDepthValue[0] -= shadowMapBias;

	//// Sample the shadow map (get depth of geometry)
	//depthValue[1] = depthMapTexture2.Sample(shadowSampler, pTexCoord).r;

	//// Calculate the depth from the light.
	//lightDepthValue[1] = input.lightViewPos2.z / input.lightViewPos2.w;
	//lightDepthValue[1] -= shadowMapBias;
	float4 lightColour;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
	if (lightDepthValue[0] < depthValue[0])
	{
		colour = calculateLighting(-direction.xyz, input.normal, diffuse);

	}

	//// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
	//if (lightDepthValue[1] < depthValue[1])
	//{
	//	lightColour = calculateLighting(-spotdirection, input.normal, diffuseColour);

	//}

	colour = saturate(colour + ambient);

	float3 lightVector = normalize(position - input.worldPosition);


	float3 inverseLightVector = -lightVector;

	float cosA = dot(inverseLightVector, spotdirection);

	float cosB = cos(angle);
	

	lightColour = ambientColor + calculateLighting(lightVector, input.normal, diffuseColour);

	//if (cosA > cosB)
	//{
	//	//return float4(1, 0, 0, 1);
	//}
	//else
	//{
	//	lightColour = ambientColor;
	//	//return float4(1, 0, 0, 1);

	//}

	float result = pow(max(cosA, cosB), angle);

	

	float4 spotlightColor = lightColour * result;

	return saturate((colour + spotlightColor) * textureColour);
	
}