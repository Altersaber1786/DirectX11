#include "GameObject.h"


GameObject::GameObject() :rotation_(0.0f, 0.0f, 0.0f), scale_(1.0f, 1.0f, 1.0f), position_(0.0f, 0.0f, 0.0f)
{ 

}

bool GameObject::Initialize(XMFLOAT3 Position, XMFLOAT3 Rotation, XMFLOAT3 Scale)
{
	
	SetPosition(Position);
	SetRotation(Rotation);
	SetScale(Scale);

	return true;
}

bool GameObject::InitWorldCB(ID3D11Device* device)
{
	HRESULT result;

	//Constant buffers description
	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = sizeof(worldRotMat);
	constDesc.Usage = D3D11_USAGE_DEFAULT;

	//world matrix
	result = device->CreateBuffer(&constDesc, 0, &m_worldRotateCB);

	if (FAILED(result))
	{
		MessageBox(NULL, "Failed to create world const buffer", NULL, NULL);
		return false;
	}
	return true;
}
void GameObject::Set3DModel(GameModel* model)
{
	m_3DModel = model;
	m_totalVertex = model->m_totalVertex;
}

void GameObject::SetPosition(XMFLOAT3 position)
{
	position_ = position;
};

void GameObject::SetRotation(XMFLOAT3 rotation)
{
	rotation_ = rotation;
};

void GameObject::SetScale(XMFLOAT3 scale)
{
	scale_ = scale;
};

XMMATRIX GameObject::GetWorldMatrix()
{
	XMMATRIX	translation	=	XMMatrixTranslation(position_.x, position_.y, position_.z);
	worldRotMat_.rotation	=	XMMatrixRotationRollPitchYaw(rotation_.x, rotation_.y, rotation_.z);
	XMMATRIX	scale		=	XMMatrixScaling(scale_.x, scale_.y, scale_.z);
	
	return	worldRotMat_.rotation*scale*translation;
};


void GameObject::Update()
{
	worldRotMat_.world = GetWorldMatrix();
	worldRotMat_.world = XMMatrixTranspose(worldRotMat_.world);
	worldRotMat_.rotation = XMMatrixTranspose(worldRotMat_.rotation);
}

void GameObject::Render(DirectDevice* device)
{
	
	device->d3dContext_->IASetVertexBuffers(0, 1, &m_3DModel->vertexBuffer_, &stride, &offset);
	device->d3dContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Update sub resources
	device->d3dContext_->UpdateSubresource(m_worldRotateCB, 0, 0, &worldRotMat_, 0, 0);
	device->d3dContext_->VSSetConstantBuffers(0, 1, &m_worldRotateCB);

	device->d3dContext_->Draw(m_totalVertex, 0);

}

void GameObject::Relase()
{
	if(m_worldRotateCB) m_worldRotateCB ->Release();
	m_worldRotateCB = 0;
	m_3DModel = 0;
};

GameObject::~GameObject()
{
	this->Relase();
}
