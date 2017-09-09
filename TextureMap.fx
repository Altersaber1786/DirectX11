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


struct VS_Input
{
    float4 pos  : POSITION;
    float2 tex0 : TEXCOORD0;
};

struct PS_Input
{
    float4 pos  : SV_POSITION;
    float2 tex0 : TEXCOORD0;
};


PS_Input VS_Main( VS_Input vertex )
{
    PS_Input vsOut = ( PS_Input )0;
    vsOut.pos = mul( vertex.pos, worldMatrix );
    vsOut.pos = mul( vsOut.pos, viewMatrix );
    vsOut.pos = mul( vsOut.pos, projMatrix );
    vsOut.tex0 = vertex.tex0;

    return vsOut;
}


float4 PS_Main( PS_Input frag ) : SV_TARGET
{
    float4 result = psResource.Sample( samplerState_, frag.tex0 );

	return result;
}