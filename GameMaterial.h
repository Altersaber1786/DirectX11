#ifndef GAME_MATERIAL_H
#define GAME_MATERIAL_H
#include <vector>
#include "DirectDevice.h"
#include "DDSTextureLoader.h"
#define MAX_TEXTURE_MAP 4   // Normal, Kd, Ks

class GameMaterial
{
public:
	typedef enum TEXTURE_MAP_TYPE
	{
		AMBIENT_MAP = 0,
		DIFFUSE_MAP = 1,
		SPECULAR_MAP = 2,
		NORMAL_MAP  = 3
	}TEXTURE_MAP_TYPE;
	GameMaterial();
	~GameMaterial();
	bool Initialize(DirectDevice*device);
	bool LoadTextureFromFileAndCombine(DirectDevice* device, std::string textureFile, TEXTURE_MAP_TYPE mapType);
	bool CombineTexture(DirectDevice* device, TEXTURE_MAP_TYPE mapType, UINT width, UINT height);
	typedef enum FILE_EXTENSION
	{
		EXTENSION_JPG = 0x0,
		EXTENSION_DDS = 0x01,
		EXTENSION_PNG = 0x02,
		EXTENSION_TGA = 0x03,
		EXTENSION_INVALID = 0x80
	}FILE_EXTENSION;
	FILE_EXTENSION getExtension(std::wstring texFile);

	struct MaterialProperties
	{
		float Ka = 1.0f;
		float Kd = 1.0f;
		float Ks = 1.0f;
		float SpecExp = 1.0f;
	}surfaceProperties;

	bool	Initialized = false;
	DirectDevice::Vertex2D		windowSquare[6];
	ID3D11Texture2D*			tex2D_[MAX_TEXTURE_MAP];
	ID3D11ShaderResourceView*	shaderResourceView_[MAX_TEXTURE_MAP];
	ID3D11RenderTargetView*		renderTargetView_[MAX_TEXTURE_MAP ];  //
	ID3D11Buffer*				propCB_;
	
	void ReleaseAfterload();
private:
	ID3D11VertexShader*		vertexShader; //
	ID3D11PixelShader*		ambPixelShader;
	ID3D11PixelShader*		diffPixelShader;
	ID3D11PixelShader*		specPixelShader;
	ID3D11InputLayout*		inputLayout;//
	ID3D11SamplerState*		samplerState_;//
	ID3D11Buffer*			squareVertexBuffer;//

	UINT				stride = sizeof(DirectDevice::Vertex2D);
	UINT				offset = 0;
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};

#endif
