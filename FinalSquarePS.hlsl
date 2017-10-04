Texture2D ColorMap : register(t0);
Texture2D Intensity : register(t1);
SamplerState colorSampler : register(s0);

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
};

float4 Powf4(float4 a, float b)
{
	float4 result;
	result.x = pow(a.x, b);
	result.y = pow(a.y, b);
	result.z = pow(a.z, b);
	result.w = a.w;
	return result;
}

float4 main(PS_INPUT frag) : SV_TARGET
{
	float4 color = ColorMap.Sample(colorSampler, frag.tex0);
	float4 intensity = Intensity.Sample(colorSampler, frag.tex0);
	color = color * intensity;
	return Powf4(color, 2.2);
}