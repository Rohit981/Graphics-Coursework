// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);


cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer TimeBuffer : register(b1)
{
	float time;
	float amplitude;
	float frequency;
	float speed;
};


struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;

};

OutputType main(InputType input)
{
	OutputType output;
	float3 center = float3(50.f, 0.f, 50.f);

	float ripple = sqrt(pow((input.position.x - center.x), 2) + pow((input.position.z - center.z), 2));


	input.position.y += amplitude * sin((ripple * frequency) + (time * -(speed)));


	float3 SouthVec, WestVec, NorthVec, EastVec, normalizedSouth, normalizedWest, normalizedNorth, normalizedEast, resultantSouth, resultantWest, resultantNorth, resultantEast, resultantVec;

	SouthVec.x = input.position.x;
	SouthVec.z = input.position.z - 1;
	SouthVec.y =  input.position.y;
	 ripple = sqrt(pow((SouthVec.x  - center.x), 2) + pow((SouthVec.z - center.z), 2));
	SouthVec.y += amplitude * sin((ripple * frequency) + (time * -(speed)));

	WestVec.x = input.position.x - 1;
	WestVec.z = input.position.z;
	WestVec.y = input.position.y;
	 ripple = sqrt(pow((WestVec.x - center.x), 2) + pow((WestVec.z - center.z), 2));
	WestVec.y += amplitude * sin((ripple * frequency) + (time * -(speed)));
	
	NorthVec.x = input.position.x;
	NorthVec.z = input.position.z + 1;
	NorthVec.y = input.position.y;
	 ripple = sqrt(pow((NorthVec.x - center.x), 2) + pow((NorthVec.z - center.z), 2));
	NorthVec.y += amplitude * sin((ripple * frequency) + (time * -(speed)));
	

	EastVec.x = input.position.x + 1;
	EastVec.z = input.position.z;
	EastVec.y = input.position.y;
	 ripple = sqrt(pow((EastVec.x - center.x), 2) + pow((EastVec.z - center.z), 2));
	EastVec.y += amplitude * sin((ripple * frequency) + (time * -(speed)));
	
	normalizedSouth = normalize(SouthVec - input.position);
	normalizedWest = normalize(WestVec - input.position);
	normalizedNorth = normalize(NorthVec - input.position);
	normalizedEast = normalize(EastVec - input.position);

	resultantSouth = cross(normalizedSouth, normalizedWest);
	resultantWest = cross(normalizedWest, normalizedNorth);
	resultantNorth = cross(normalizedNorth, normalizedEast);
	resultantEast = cross(normalizedEast, normalizedSouth);

	input.normal = (resultantSouth + resultantWest + resultantNorth + resultantEast) / 4;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);


	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;


	// Calculate the normal vector against the world matrix only and normalise.
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	return output;
}