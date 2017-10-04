#include "GameModel.h"



GameModel::GameModel()
{
	m_totalVertex = 0;
}

bool GameModel::LoadGeometry(DirectDevice* device, char* meshFile)
{
	//Load Vertex buffer
	MeshLoader* m_Loader = new MeshLoader();
	if (!(m_Loader->LoadMeshFromOBJ(meshFile, device, &vertexBuffer_)))
	{
		MessageBox(NULL, "Error creating vertex buffer", NULL, NULL);
		return false;
	}
	m_totalVertex = m_Loader->GetTotalVertices();

	delete m_Loader;

	return true;
}

GameModel::~GameModel()
{
	vertexBuffer_->Release();
}
