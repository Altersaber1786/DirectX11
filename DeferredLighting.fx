Texture2D<float4>	Color : register(t0);
Texture2D<float4>	Positions : register(t1);
Texture2D<float4>	Normals  : register(t2);
Texture2D<float4>	DiffSpecCoe : register(t3);
Texture2D<float4>	Intensity : register(t4);

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

PS_INPUT VS_Main(VS_INPUT vertex)
{
	PS_INPUT vsOut = (PS_INPUT)0;
	vsOut.pos = vertex.pos;
	vsOut.tex0 = vertex.tex0;

	return vsOut;
}

float4 calcLighting(float4 position, float4 normal, float4 diffspeccoe)
{
	float4 finalInten;
	float Kd = diffspeccoe.x;
	float Ks = diffspeccoe.y;
	float SpecExp = diffspeccoe.z;

	float4 DirToLight = PointLightPosition - position;
	float distance = length(DirToLight.xyz);
	DirToLight = DirToLight / distance;
	distance = pow(distance, 2);

	//Intensity of the diffuse light. Saturate to keep within the 0-1 range.
	float NdotL = dot(normal, DirToLight);
	float intensity = saturate(NdotL);

	// Calculate the diffuse light factoring in light color, power and the attenuation
	finalInten = intensity * PointLightIntensity * Kd ;

	//Specular
	float3 viewDir = normalize(Eyepos.xyz - position.xyz);
	float3 H = normalize(viewDir.xyz + DirToLight.xyz);
	//Intensity of the specular light
	float NdotH = dot(normal, H);
	intensity = pow(saturate(NdotH), SpecExp);

	//Sum up the specular light factoring
	finalInten +=  intensity * Ks * PointLightIntensity;
	
	return finalInten;
}

float4 PS_Main(PS_INPUT frag) : SV_TARGET
{
	float4 t_position = Positions.Sample(PointSampler, frag.tex0);
	float4 t_normal = Normals.Sample(PointSampler, frag.tex0);
	float4 t_intensity = Intensity.Sample(PointSampler, frag.tex0);
	float4 t_diffspeccoe = DiffSpecCoe.Sample(PointSampler, frag.tex0);
	t_intensity = t_intensity + calcLighting(t_position, t_normal, t_diffspeccoe);

	return t_intensity;
}