Texture2D diffAmbBaseMap : register(t0);
Texture2D diffuseMap : register(t1);
Texture2D specularMap : register(t2);
Texture2D diffuseIntensity : register(t3);
Texture2D specularIntensity : register(t4);
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
	float4 baseColor = diffAmbBaseMap.Sample(colorSampler, frag.tex0);
	float4 diffColor = diffuseMap.Sample(colorSampler, frag.tex0) * diffuseIntensity.Sample(colorSampler,frag.tex0);
	float4 specColor = specularMap.Sample(colorSampler, frag.tex0) * specularIntensity.Sample(colorSampler,frag.tex0);
	baseColor = baseColor + diffColor + specColor;
	return baseColor;
}