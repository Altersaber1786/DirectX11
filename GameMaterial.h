#ifndef GAME_MATERIAL_H
#define GAME_MATERIAL_H
#include <vector>
#include "DirectDevice.h"
#include "DDSTextureLoader.h"
class GameMaterial
{
public:
	GameMaterial();
	~GameMaterial();
	bool LoadContent(DirectDevice* device, std::string textureFile);
	typedef enum FILE_EXTENSION
	{
		EXTENSION_JPG = 0x0,
		EXTENSION_DDS = 0x01,
		EXTENSION_PNG = 0x02,
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

	ID3D11ShaderResourceView*	shaderResourceView_;
	ID3D11Buffer*				propCB_;
};

#endif
