#ifndef GAME_MODEL_H
#define GAME_MODEL_H

#include "DirectDevice.h"
#include "DDSTextureLoader.h"
#include "GameMaterial.h"
#include "GameObject.h"
#include <vector>
class Box
{
public:
	Box();
	~Box();
	UINT				StartVertex; //The begin of buffer is vertex[0]
	UINT				NumVertex;
	GameMaterial*		material;
	
	void Release();
	void Render(DirectDevice* device, ID3D11Buffer* vertexBuffer);
};

class GameModel
{
public:
	typedef struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 texcoord;
		XMFLOAT3 normal;
	}Vertex;

	GameModel();
	~GameModel();
	bool pushObject(GameObject* obj);
	void Render(DirectDevice* device, XMMATRIX& viewMat, XMMATRIX& projMat);
	void Update();

	bool	isTransparent;
	UINT	totalObjects;
	UINT	index;
	UINT	m_totalVertices;
	std::vector<GameObject*>	ObjectList;
	std::vector<Box>			BoxList;
	ID3D11Buffer*				vertexBuffer_;
	UINT		stride = sizeof(Vertex);
	UINT		offset = 0;
	void Release();
};

#endif