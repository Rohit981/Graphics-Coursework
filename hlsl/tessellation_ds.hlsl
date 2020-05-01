// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;	
};

cbuffer TimeBuffer : register(b1)
{
	float time;
	float amplitude;
	float frequency;
	float speed;
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
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos : TEXCOORD1;
	float4 lightViewPos2 : TEXCOORD2;
	float4 worldPosition : TEXCOORD3;
};





[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	float3 vertexPosition;
	float2 texturePosition;
	float3 normalPosition;

	OutputType output;

	// Determine the position of the new vertex.
	// Invert the y and Z components of uvwCoord as these coords are generated in UV space and therefore y is positive downward.
	// Alternatively you can set the output topology of the hull shader to cw instead of ccw (or vice versa).
	float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
	float3 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
	vertexPosition = lerp(v1, v2, uvwCoord.x);
	//output.position = vertexPosition;

	float2 texture1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
	float2 texture2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
	texturePosition = lerp(texture1, texture2, uvwCoord.x);
	output.tex = texturePosition;

	float3 normal1 = lerp(patch[0].normal, patch[1].normal, uvwCoord.y);
	float3 normal2 = lerp(patch[3].normal, patch[2].normal, uvwCoord.y);
	normalPosition = lerp(normal1, normal2, uvwCoord.x);
	output.normal = normalPosition;

	

	float textureColor = texture0.SampleLevel(sampler0, output.tex, 0);
	float offest = (textureColor * 35.f);
	vertexPosition.y += output.normal.y * offest;

	float2 pixelSize = float2(1.f / 1024, 1.f / 1024);

	float2 offset00 = output.tex + float2(-pixelSize.x, -pixelSize.y);
	float2 offset010 = output.tex + float2(0.f, -pixelSize.y);
	float2 offset020 = output.tex + float2(pixelSize.x, -pixelSize.y);

	float2 offset01 = output.tex + float2(-pixelSize.x, 0.f);
	float2 offset021 = output.tex + float2(pixelSize.x, 0.f);

	float2 offset002 = output.tex + float2(-pixelSize.x, pixelSize.y);
	float2 offset012 = output.tex + float2(0.f, -pixelSize.y);
	float2 offset022 = output.tex + float2(pixelSize.x, pixelSize.y);

	float h00 = 3 * texture0.SampleLevel(sampler0, offset00, 0.f).r;
	float h10 = 3 * texture0.SampleLevel(sampler0, offset010, 0.f).r;
	float h20 = 3 * texture0.SampleLevel(sampler0, offset020, 0.f).r;

	float h01 = 3 * texture0.SampleLevel(sampler0, offset01, 0.f).r;
	float h21 = 3 * texture0.SampleLevel(sampler0, offset021, 0.f).r;

	float h02 = 3 * texture0.SampleLevel(sampler0, offset002, 0.f).r;
	float h12 = 3 * texture0.SampleLevel(sampler0, offset012, 0.f).r;
	float h22 = 3 * texture0.SampleLevel(sampler0, offset022, 0.f).r;

	//Evaluate the Sobel Filter
	float X = h00 - h20 + 2.f * h01 - 2.f * h21 + h02 - h22;
	float Y = h00 + 2.f * h10 + h20 - h02 - 2.f * h12 - h22;

	float Z = 0.5f * sqrt(max(0.f, 1.f - X * X - Y * Y));


	output.normal = normalize(float3(2.f * X, Z, 2.f * Y));

	//ssvertexPosition.z = amplitude * sin((vertexPosition.x* frequency) + (time * speed));

	// Calculate the position of the new vertex against the world, view, and projection matrices.
	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Calculate the position of the vertice as viewed by the light source.
	output.lightViewPos = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.lightViewPos = mul(output.lightViewPos, lightViewMatrix);
	output.lightViewPos = mul(output.lightViewPos, lightProjectionMatrix);

	
	output.worldPosition = mul(float4(vertexPosition, 1.0f), worldMatrix);

	// Send the input color into the pixel shader.
	//output.tex = patch[0].colour;

	return output;
}

