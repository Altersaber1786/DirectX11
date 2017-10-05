Texture2D colorMap : register(t0);
SamplerState samplerState : register(s0);


cbuffer cbChangesEveryFrame : register(b0) //Handle by Object model
{
	matrix worldMatrix;
	matrix rotationMatrix;
};

cbuffer cbChangeOnCamAction : register(b1) //Handle by Camera
{
	matrix viewMatrix;
};

cbuffer cbChangeOnResize : register(b2) //Handle by Direct device
{
	matrix projMatrix;
};

cbuffer DiffuseAmbient : register(b0)
{
	float4	AmbientDown;
	float4	AmbientRange;
	float4	DiffuseIntensity;
	float4	DiffuseDirection;
};

cbuffer MaterialProperties : register(b1)
{
	float	Ka;
	float	Kd;
	float	Ks;
	float	SpecExp;
};

struct VS_INPUT
{
	float4 pos : POSITION;
	float2 tex0 : TEXCOORD0;
	float4 norm : NORMAL0;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float4 norm : NORMAL0;
	float3 worldPos : TEXCOORD1;
};

struct PS_OUTPUT
{
	float4 BaseColor : SV_TARGET0;
	float4 Position  : SV_TARGET1;
	float4 Normal    : SV_TARGET2;
	float4 DiffSpecCoe : SV_TARGET3;
	float4 Intensity : SV_TARGET4;
};


PS_INPUT VS_Main(VS_INPUT vertex)
{
	PS_INPUT vsOut = (PS_INPUT)0;
	vsOut.pos = mul(vertex.pos, worldMatrix);
	vsOut.worldPos = vsOut.pos.xyz;
	vsOut.pos = mul(vsOut.pos, viewMatrix);
	vsOut.pos = mul(vsOut.pos, projMatrix);
	vsOut.tex0 = vertex.tex0;
	vsOut.norm = mul(vertex.norm, rotationMatrix);
	return vsOut;
}

float4 Powf4(float4 a, float b)
{
	float4 result;
	result.x = pow(a.x, b);
	result.y = pow(a.y, b);
	result.z = pow(a.z, b);
	result.w = a.w;
	return result;
}

float4 calcAmbient(float4 normal)
{

	float up = normal.y*0.5 + 0.5;

	float4 Ambient = AmbientDown + up * AmbientRange;

	return Ambient*Ka;
}

float4 calcDiffuse(float4 normal)
{
	float4 DirToDiffuse = DiffuseDirection;
	DirToDiffuse = -DirToDiffuse;
	float  NDotL = dot(DirToDiffuse, normal);
	float4 finalInten = Kd * DiffuseIntensity * saturate(NDotL);

	return finalInten;
}

PS_OUTPUT PS_Main(PS_INPUT frag) : SV_TARGET
{
	PS_OUTPUT result;
	//result.BaseColor = colorMap.Sample(samplerState, frag.tex0);
	//result.BaseColor = Powf4(result.BaseColor, 1.0 / 2.2);
	result.BaseColor = float4 (1.0f, 1.0f, 1.0f, 1.0f);
	result.Position = float4(frag.worldPos, 1.0f);
	result.Normal = frag.norm;
	result.Intensity = calcAmbient(frag.norm) + calcDiffuse(frag.norm);
	result.DiffSpecCoe.x = Kd;
	result.DiffSpecCoe.y = Ks;
	result.DiffSpecCoe.z = SpecExp;
	return result;
}