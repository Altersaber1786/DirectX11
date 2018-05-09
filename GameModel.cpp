#include "GameModel.h"


Box::Box()
{
}

GameModel::GameModel()
{
	m_totalVertices = 0;
}



bool GameModel::pushObject(GameObject* obj)
{
	ObjectList.push_back(obj);
	totalObjects = static_cast<unsigned int>(ObjectList.size());
	return true;
};
void GameModel::Update()
{

	for (UINT i = 0; i < totalObjects; i++)
	{
		ObjectList[i]->Update();
	}

}

void GameModel::Render(DirectDevice* device, XMMATRIX& viewMat, XMMATRIX& projMat)
{
	for (UINT i = 0; i < ObjectList.size(); i++)
	{
		device->d3dContext_->IASetVertexBuffers(0, 1, &vertexBuffer_, &stride, &offset);
		ObjectList[i]->PrepareRender(device, viewMat, projMat);
		for (UINT j = 0; j < BoxList.size(); j++)
		{
			BoxList[j].Render(device, vertexBuffer_);
		}
	};
};

void Box::Render(DirectDevice* device, ID3D11Buffer* vertexBuffer)
{
	for (int i = 0; i < MAX_TEXTURE_MAP; i++)
	{
		if (material->shaderResourceView_[i] != nullptr)
			device->d3dContext_->PSSetShaderResources(i, 1, &material->shaderResourceView_[i]);
	}
	device->d3dContext_->PSSetConstantBuffers(1, 1, &material->propCB_);
	device->d3dContext_->Draw(NumVertex, StartVertex);
}

void Box::Release()
{
	delete material;
}
void GameModel::Release()
{
	for (int i = BoxList.size() - 1; i > 0; i--)
	{
		BoxList[i].Release();
	}
	BoxList.clear();
	if (vertexBuffer_!= nullptr) vertexBuffer_->Release();
	vertexBuffer_ = nullptr;
	for (int i = ObjectList.size() - 1; i > 0; i--)
	{
		ObjectList[i]->Release();
	}
	ObjectList.clear();
}

GameModel::~GameModel()
{
	this->Release();
}

Box::~Box() 
{
}