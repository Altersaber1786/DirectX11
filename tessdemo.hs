cbuffer TessFactors : register(b0)
{
	float tessellationAmount;
	float3 padding;
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


HS_CONSTANT_DATA_OUTPUT ConstantsHS()
{
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
[maxtessfactor(10.0f)]
[patchconstantfunc("ConstantsHS")] 

HS_CONTROL_POINT_OUTPUT HS(InputPatch<VS_CONTROL_POINT_OUTPUT, 3> inputPatch, uint uCPID : SV_OutputControlPointID, uint patchId : SV_PrimitiveID )
{
	HS_CONTROL_POINT_OUTPUT Output;
	Output.vWorldPos = inputPatch[uCPID].vWorldPos;
	Output.vTexCoord = inputPatch[uCPID].vTexCoord; 
	Output.vNormal = inputPatch[uCPID].vNormal; 
	
	return Output;
};

