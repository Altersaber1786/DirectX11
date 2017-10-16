/*
    Beginning DirectX 11 Game Programming
    By Allen Sherrod and Wendy Jones

    Texture Mapping Shader for 3D Objects
*/


Texture2D psResource : register( t0 );
SamplerState samplerState_ : register( s0 );


struct VS_Input
{
    float4 pos  : POSITION0;
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
	vsOut.pos = vertex.pos;
	vsOut.tex0 = vertex.tex0;

    return vsOut;
}

float4 PS_Main( PS_Input frag ) : SV_TARGET
{
	return  psResource.Sample(samplerState_, frag.tex0);;
}