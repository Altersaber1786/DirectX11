Texture2D<float4>	Positions : register(t0);
Texture2D<float4>	Normals  : register(t1);
Texture2D<float4>	diffuseIntensity : register(t2);
Texture2D<float4>	specularIntensity : register(t3);

SamplerState PointSampler : register(s0);

cbuffer LightSourceProperties : register(b0)
{
	float4	PointLightPosition;
	float4	PointLightIntensity;
}

cbuffer CamPosCB : register(b1)
{
	float4 Eyepos;
}

struct VS_INPUT
{
	float4 pos : POSITION;
	float2 tex0 : TEXCOORD0;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
};



struct PS_OUTPUT
{
	float4 DiffuseIntensity : SV_TARGET0;
	float4 SpecIntensity : SV_TARGET1;
};

PS_INPUT VS_Main(VS_INPUT vertex)
{
	PS_INPUT vsOut = (PS_INPUT)0;
	vsOut.pos = vertex.pos;
	vsOut.tex0 = vertex.tex0;

	return vsOut;
}

float4 calcDiffuse(float4 position, float4 normal)
{
	float4 finalInten;

	float SpecExp = normal.w;

	float3 DirToLight = PointLightPosition.xyz - position.xyz;
	float distance = length(DirToLight);
	DirToLight = DirToLight / distance;
	distance = pow(distance, 2.0f);

	//Intensity of the diffuse light. Saturate to keep within the 0-1 range.
	float NdotL = dot(normal.xyz, DirToLight);
		  NdotL = saturate(NdotL);

	// Calculate the diffuse light factoring in light color, power and the attenuation
	finalInten = NdotL * PointLightIntensity;
	return finalInten;
};

float4 calcSpec(float4 position, float4 normal)
{
	float4 finalInten;

	float SpecExp = normal.w;

	float3 DirToLight = PointLightPosition.xyz - position.xyz;
	float distance = length(DirToLight);
	DirToLight = DirToLight / distance;

	//Specular
	float3 viewDir = normalize(Eyepos.xyz - position.xyz);
	float3 H = normalize(viewDir + DirToLight);
	//Intensity of the specular light
	float NdotH = dot(normal.xyz, H);
	NdotH = pow(saturate(NdotH), SpecExp);

	//Sum up the specular light factoring
	finalInten =  NdotH * PointLightIntensity;
	
	return finalInten;
}

PS_OUTPUT PS_Main(PS_INPUT frag) : SV_TARGET
{
	PS_OUTPUT pOut;
	float4 t_position = Positions.Sample(PointSampler, frag.tex0);
	float4 t_normal = Normals.Sample(PointSampler, frag.tex0);
	float4 t_SpecIntensity = specularIntensity.Sample(PointSampler, frag.tex0);
	float4 t_DiffuseIntensity = diffuseIntensity.Sample(PointSampler, frag.tex0);
	

	pOut.SpecIntensity = float4(1.0f, 1.0f, 1.0f, 1.0f);
	pOut.DiffuseIntensity = float4(1.0f, 1.0f, 1.0f, 1.0f);
	
	return pOut;
}