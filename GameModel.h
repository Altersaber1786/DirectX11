#ifndef GAME_MODEL_H
#define GAME_MODEL_H

#include "DirectDevice.h"
#include "MeshLoader.h"
#include "DDSTextureLoader.h"
#include <vector>
class GameModel
{
public:
	GameModel();
	~GameModel();
	bool LoadGeometry(DirectDevice* device, char* meshFile);	

	int index;

	ID3D11Buffer*		vertexBuffer_;
	
	int					m_totalVertex;

	struct MaterialProperties
	{
		float Ka = 1.0f;
		float Kd = 1.0f;
		float Ks = 1.0f;
		float SpecExp = 1.0f;
	};
	struct Box
	{
		ID3D11Texture2D* tex;
		UINT startVertex;
		UINT numVertex;
		MaterialProperties matProp;
	};
	std::vector<Box> box;
};

#endif