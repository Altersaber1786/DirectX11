#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <stdio.h>
#include <string>
#include "DirectDevice.h"
class GameObject
{
public:
//Attributes
	std::string	name;
	UINT		modelIndex;
	XMFLOAT3	position_;
	XMFLOAT3	rotation_;
	XMFLOAT3	scale_;
	float		speed;

	GameObject();
	~GameObject();
	
	const XMFLOAT3 up = {0.0f, 1.0f, 0.0f};
	bool		Initialize(XMFLOAT3 Position, XMFLOAT3 Rotation, XMFLOAT3 Scale);
	bool		InitWorldCB(ID3D11Device* device);
	void		SetPosition(XMFLOAT3 position);
	void		SetRotation(XMFLOAT3 rotation);
	void		SetScale(XMFLOAT3 scale);
	void		Update();
	void		PrepareRender(DirectDevice* device, XMMATRIX& viewMat, XMMATRIX& projMat);
	void		Release();
	XMMATRIX	GetWorldMatrix();

private:
	int m_totalVertex;
	

	XMMATRIX	rotationMat;

	struct WorldViewProj
	{
		XMMATRIX	world;
		XMMATRIX	view;
		XMMATRIX	project;
	}WorldViewProj_;

	ID3D11Buffer*				m_RotateCB;
	ID3D11Buffer*				m_WorldViewProjCB;
};

#endif