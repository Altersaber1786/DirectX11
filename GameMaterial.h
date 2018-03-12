#ifndef GAME_MATERIAL_H
#define GAME_MATERIAL_H
#include <vector>
#include "GameObject.h"
#include "DirectDevice.h"

class GameMaterial
{
public:
	GameMaterial();
	~GameMaterial();
	bool LoadContent(DirectDevice* device, wchar_t* textureFile);
	void Render(DirectDevice* device, XMMATRIX viewMat, XMMATRIX projMat);
	void Update();
	UINT	totalObjects;
	bool pushObject(GameObject* obj);

	std::vector<GameObject*>	ObjectList;

	typedef enum FILE_EXTENSION
	{
		EXTENSION_JPG = 0x0,
		EXTENSION_DDS = 0x01,
		EXTENSION_PNG = 0x02,
		EXTENSION_INVALID = 0x80
	}FILE_EXTENSION;
	FILE_EXTENSION getExtension(wchar_t* texFile);

private:
	

	struct MaterialProperties
	{
		float Ka = 1.0f;
		float Kd = 1.0f;
		float Ks = 1.0f;
		float SpecExp = 64.0f;
	}surfaceProperties;



	ID3D11InputLayout*			inputLayout_;
	ID3D11PixelShader*			pixelShader_;
	ID3D11VertexShader*			vertexShader_;
	ID3D11ShaderResourceView*	shaderResourceView_;
	ID3D11SamplerState*			samplerState_;
	ID3D11Buffer*				propCB_;
};

#endif
