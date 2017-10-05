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
	void Render(DirectDevice* device);
	void Update();
	int	totalObjects;
	bool pushObject(GameObject* obj);

	std::vector<GameObject*>	ObjectList;

private:
	struct MaterialProperties
	{
		float Ka = 1.0f;
		float Kd = 1.0f;
		float Ks = 1.0f;
		float SpecExp = 40.0f;
	}surfaceProperties;

	ID3D11InputLayout*			inputLayout_;
	ID3D11PixelShader*			pixelShader_;
	ID3D11VertexShader*			vertexShader_;
	ID3D11ShaderResourceView*	shaderResourceView_;
	ID3D11SamplerState*			samplerState_;
	ID3D11Buffer*				propCB_;
};

#endif
