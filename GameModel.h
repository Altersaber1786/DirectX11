#ifndef GAME_MODEL_H
#define GAME_MODEL_H

#include "DirectDevice.h"
#include "MeshLoader.h"
#include "DDSTextureLoader.h"

class GameModel
{
public:
	GameModel();
	~GameModel();
	bool LoadGeometry(DirectDevice* device, char* meshFile);	

	int index;

	ID3D11Buffer*		vertexBuffer_;
	
	int					m_totalVertex;
};

#endif