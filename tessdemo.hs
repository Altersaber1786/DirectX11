static const float d0 = 2.0f;
static const float d1 = 200.0f;
cbuffer TessFactors : register(b0)
{
	float4 EyePos;
};

struct VS_CONTROL_POINT_OUTPUT
{
	float3 vWorldPos : POSITION;
	float2 vTexCoord : TEXCOORD0;
	float3 vNormal : NORMAL;
};

struct HS_CONTROL_POINT_OUTPUT
{
	float3 vWorldPos : POSITION;
	float2 vTexCoord : TEXCOORD0;
	float3 vNormal : NORMAL;
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float Edges[3] : SV_TessFactor;
	float Inside : SV_InsideTessFactor;
};


HS_CONSTANT_DATA_OUTPUT ConstantsHS(InputPatch<VS_CONTROL_POINT_OUTPUT, 3> inputPatch, uint patchId : SV_PrimitiveID )
{
	float3 center = (inputPatch[0].vWorldPos + inputPatch[1].vWorldPos + inputPatch[2].vWorldPos)/3;
	float d = distance(center, EyePos.xyz);
	
	float tessellationAmount = 1.0f*saturate((d1-d)/(d1-d0));
	
	
	HS_CONSTANT_DATA_OUTPUT Output;
	Output.Edges[0] = tessellationAmount;
	Output.Edges[1] =  tessellationAmount; 
	Output.Edges[2] = tessellationAmount; 
	Output.Inside   = tessellationAmount; 
	return Output;
};

[domain("tri")] 
[partitioning("fractional_odd")] 
[outputtopology("triangle_cw")] 
[outputcontrolpoints(3)] 
[maxtessfactor(32.0f)]
[patchconstantfunc("ConstantsHS")] 

HS_CONTROL_POINT_OUTPUT HS(InputPatch<VS_CONTROL_POINT_OUTPUT, 3> inputPatch, uint uCPID : SV_OutputControlPointID, uint patchId : SV_PrimitiveID )
{
	HS_CONTROL_POINT_OUTPUT Output;
	Output.vWorldPos = inputPatch[uCPID].vWorldPos;
	Output.vTexCoord = inputPatch[uCPID].vTexCoord; 
	Output.vNormal = inputPatch[uCPID].vNormal; 
	
	return Output;
};

