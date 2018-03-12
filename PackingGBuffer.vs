
cbuffer cbChangesEveryFrame : register(b0) //Handle by Object model
{
	matrix rotationMatrix;
};

struct VS_INPUT
{
	float4 pos : POSITION;
	float2 tex0 : TEXCOORD0;
	float4 norm : NORMAL;
};

struct VS_OUTPUT
{
	float3 pos : POSITION;
	float2 tex0 : TEXCOORD0;
	float3 norm : NORMAL;
};


VS_OUTPUT VS_Main(VS_INPUT vertex)
{
	VS_OUTPUT vsOut = (VS_OUTPUT)0;
	vsOut.pos = vertex.pos;
	vsOut.tex0 = vertex.tex0;
	float4 norm = mul(vertex.norm, rotationMatrix);
	vsOut.norm = norm.xyz;

	return vsOut;
}
