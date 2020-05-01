// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;	
};


struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;


};

struct OutputType
{
	float4 position : SV_POSITION;	
	float4 depthPosition : TEXCOORD1;
};



	
	


[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	float3 vertexPosition;
	float2 texturePosition;
	float3 normalPosition;

	OutputType output;

	 //Determine the position of the new vertex.
	 //nvert the y and Z components of uvwCoord as these coords are generated in UV space and therefore y is positive downward.
	 //Alternatively you can set the output topology of the hull shader to cw instead of ccw (or vice versa).
	float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
	float3 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
	vertexPosition = lerp(v1, v2, uvwCoord.x);
	output.position = float4(vertexPosition, 1);

	float2 texture1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
	float2 texture2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
	texturePosition = lerp(texture1, texture2, uvwCoord.x);
	//output.tex = texturePosition;

	float3 normal1 = lerp(patch[0].normal, patch[1].normal, uvwCoord.y);
	float3 normal2 = lerp(patch[3].normal, patch[2].normal, uvwCoord.y);
	normalPosition = lerp(normal1, normal2, uvwCoord.x);
	//output.normal = normalPosition;
	

	float textureColor = texture0.SampleLevel(sampler0, texturePosition, 0);
	float offest = (textureColor * 35.f);
	output.position.y += normalPosition.y * offest;



	// Calculate the position of the new vertex against the world, view, and projection matrices.
	output.position = mul(output.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the position value in a second input value for depth value calculations.
	output.depthPosition = output.position;

	// Send the input color into the pixel shader.
	//output.tex = patch[0].colour;

	return output;
}

