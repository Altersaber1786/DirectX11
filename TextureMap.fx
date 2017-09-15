/*
    Beginning DirectX 11 Game Programming
    By Allen Sherrod and Wendy Jones

    Texture Mapping Shader for 3D Objects
*/


Texture2D psResource : register( t0 );
SamplerState samplerState_ : register( s0 );


cbuffer cbChangesEveryFrame : register( b0 ) //Handle by Object model
{
    matrix worldMatrix;
};

cbuffer cbChangeOnCamAction : register( b1 ) //Handle by Camera
{
    matrix viewMatrix;
};

cbuffer cbChangeOnResize : register( b2 ) //Handle by Direct device
{
    matrix projMatrix;
};

cbuffer cbHemiCorlor : register(b0)
{
	float4 AmbientDown;
	float4 AmbientRange;
};

cbuffer DirLightConstants : register (b1)
{
	float4 DirToLight;
	float4 DirLightColor;
}

struct VS_Input
{
    float4 pos  : POSITION0;
    float2 tex0 : TEXCOORD0;
	float4 norm : NORMAL0;
};

struct PS_Input
{
    float4 pos  : SV_POSITION;
    float2 tex0 : TEXCOORD0;
	float4 norm : NORMAL0;
};



PS_Input VS_Main( VS_Input vertex )
{
    PS_Input vsOut = ( PS_Input )0;
    vsOut.pos = mul( vertex.pos, worldMatrix );
    vsOut.pos = mul( vsOut.pos, viewMatrix );
    vsOut.pos = mul( vsOut.pos, projMatrix );
    vsOut.tex0 = vertex.tex0;
	vsOut.norm = mul(vertex.norm, worldMatrix);

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

float4 calcAmbient(float4 normal, float4 color)
{
	float up = normal.y*0.5 + 0.5;
	
	float4 Ambient = AmbientDown + up * AmbientRange;
	float4 tempColor = Powf4(color, 1.0 / 2.2);
	color = Powf4(Ambient*tempColor, 2.2);
	return color;
}

float4 PS_Main( PS_Input frag ) : SV_TARGET
{
    float4 result = psResource.Sample( samplerState_, frag.tex0 );
	result = calcAmbient(frag.norm, result);
	
	return result;
}