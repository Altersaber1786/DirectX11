#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "DirectDevice.h"
#include "GameModel.h"

class GameObject
{
public:
//Attributes
	std::string	name;
	int			modelIndex;
	XMFLOAT3	position_;
	XMFLOAT3	rotation_;
	XMFLOAT3	scale_;

	GameObject();
	~GameObject();
	
	const XMFLOAT3 up = {0.0f, 1.0f, 0.0f};
	bool		Initialize(XMFLOAT3 Position, XMFLOAT3 Rotation, XMFLOAT3 Scale);
	bool		InitWorldCB(ID3D11Device* device);
	void		SetPosition(XMFLOAT3 position);
	void		SetRotation(XMFLOAT3 rotation);
	void		SetScale(XMFLOAT3 scale);
	void		Update();
	void		Render(DirectDevice* device);
	void		Relase();
	XMMATRIX	GetWorldMatrix();
	void		Set3DModel(GameModel* model);

private:
	int m_totalVertex;
	unsigned int stride = sizeof(MeshLoader::Vertex);
	unsigned int offset = 0;
	
	struct worldRotMat
	{
		XMMATRIX	world;
		XMMATRIX	rotation;
	}worldRotMat_;

	GameModel*					m_3DModel;
	ID3D11Buffer*				m_worldRotateCB;
	
};

#endif