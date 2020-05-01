// Tessellation Hull Shader
// Prepares control points for tessellation

cbuffer TessBuffer : register(b0)
{
	float4 tess;
	float2 insideFactor;
	float2 padding;
	float4 camPosition;
};
struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	
};

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;
	float maxTess = 64;


	/*inputPatch[patchid + 1];
	inputPatch.edges*/

	float3 midPoint1 = float3(inputPatch[0].position + inputPatch[1].position) / 2;
	float3 midPoint2 = float3(inputPatch[1].position + inputPatch[2].position) / 2;
	/*float3 midPoint3 = float3(inputPatch[2] + inputPatch[3]) / 2;
	float3 midPoint4 = float3(inputPatch[3] + inputPatch[0]) / 2;*/

	float3 midPoint = float3(inputPatch[0].position + inputPatch[1].position + inputPatch[2].position + inputPatch[3].position) / 4;

	float3 cam = float3(camPosition.x - midPoint.x, camPosition.y - midPoint.y, camPosition.z - midPoint.z);
	float3 cameEdgeX = float3(camPosition.x - midPoint1.x, camPosition.y - midPoint1.y, camPosition.z - midPoint1.z);
	float3 cameEdgeY = float3(camPosition.x - midPoint2.x, camPosition.y - midPoint2.y, camPosition.z - midPoint2.z);


	float tessFactor = maxTess - (length(cam) * 4);
	float tessFactorX = maxTess - (length(cameEdgeX));
	float tessFactorY = maxTess - (length(cameEdgeY));

	if (tessFactor < 1)
	{
		tessFactor = 1;
	}
	if (tessFactorX < 1)
	{
		tessFactorX = 1;
	}
	if (tessFactorY < 1)
	{
		tessFactorY = 1;
	}
	// Set the tessellation factors for the three edges of the triangle.
	output.edges[0] = tessFactorX;
	output.edges[1] = tessFactorY;
	output.edges[2] = tessFactorX;
	output.edges[3] = tessFactorY;

	// Set the tessella factor for tessallating inside the triangle.
	output.inside[0] = tessFactor;
	output.inside[1] = tessFactor;


	return output;
}


[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
	OutputType output;


	// Set the position for this control point as the output position.
	output.position = patch[pointId].position;

	// Set the input colour as the output colour.
	output.tex = patch[pointId].tex;

	output.normal = patch[pointId].normal;
	

	return output;
}