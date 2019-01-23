Texture2D LightColorMap : register(t0);
SamplerState samplerState : register(s0);

cbuffer MaterialProperties : register(b0)
{
  float Ka;
  float Kd;
  float Ks;
  float Ns;
};

struct VS_INPUT
{
	float4 pos : POSITION;
	float2 tex0 : TEXCOORD0;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0: TEXCOORD0;
};
PS_INPUT VS_MAIN(VS_INPUT vertex)
{
	PS_INPUT vertexOut = (PS_INPUT)0;
	vertexOut.pos = vertex.pos;
	vertexOut.tex0 = vertex.tex0;
	return vertexOut;
}

float4 PS_AMB(PS_INPUT input) : SV_TARGET
{
	float4 color = LightColorMap.Sample(samplerState, input.tex0);
	color = color*Ka;
	return color;
}

float4 PS_DIFF(PS_INPUT input) : SV_TARGET
{
	float4 color = LightColorMap.Sample(samplerState, input.tex0);
	color = color*Kd;
	return color;
}

float4 PS_SPEC(PS_INPUT input) : SV_TARGET
{
	float4 color = LightColorMap.Sample(samplerState, input.tex0);
	color = color*Ks;
	return color;
}