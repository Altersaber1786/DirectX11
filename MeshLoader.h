#ifndef MESH_LOADER
#define MESH_LOADER

#include "DirectDevice.h"
#include <iostream>
#define TOKEN_INVALID 0
#define TOKEN_V		 1
#define TOKEN_VT	 2
#define TOKEN_VN	 3
#define TOKEN_F		 4
#define TOKEN_MTLLIB 5
#define TOKEN_USEMTL 6
#define TOKEN_HASH   7
#define TOKEN_EMPTY_LINE   8
#define TOKEN_G      9
#define TOKEN_S		 10

class MeshLoader
{
public:
	
	int GetTotalVertices()
	{
		return totalVetices;
	};
	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 texcoord;
		XMFLOAT3 normal;
	};
	struct Indices
	{
		int vertexId;		
		int textureId;
		int normalId;
	};
	struct Face
	{
		Indices Id[3];
	};

	MeshLoader();
	~MeshLoader();
	char mtllib[6] = { 'm', 't', 'l', 'l', 'i', 'b' };
	char usemtl[6] = { 'u','s', 'e', 'm', 't', 'l' };
	bool LoadMeshFromOBJ(char* filename, DirectDevice* device, ID3D11Buffer** vertexBuffer);
	int getToken(char* buffer, int& startIndex, int& endIndex);
	float getFloat(char* buffer, int& startIndex, int& endIndex);
private:
	int totalVetices;
};

#endif

