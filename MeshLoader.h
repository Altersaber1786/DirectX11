#ifndef MESH_LOADER
#define MESH_LOADER

#include "DirectDevice.h"
#include "GameModel.h"
#include <iostream>
#define TOKEN_INVALID 0
#define TOKEN_V		 1
#define TOKEN_VT	 2
#define TOKEN_VN	 3
#define TOKEN_F3	 4
#define TOKEN_MTLLIB 5
#define TOKEN_USEMTL 6
#define TOKEN_HASH   7
#define TOKEN_EMPTY_LINE   8
#define TOKEN_G      9
#define TOKEN_S		 10
#define TOKEN_O		 11
#define TOKEN_F4	 12
#define TOKEN_NEWMTL 13
#define TOKEN_ILLUM  14
#define TOKEN_KD	 15
#define TOKEN_KA	 16
#define TOKEN_TF     17
#define TOKEN_MAP_KD 18
#define TOKEN_NI     19
#define TOKEN_KS     20
#define TOKEN_NS	 21
#define TOKEN_MAP_KA 22
#define TOKEN_MAP_KS 23
#define TOKEN_REFL	 24
#define TOKEN_BUMP   25
#define TOKEN_MAP_D  26
#define TOKEN_D		 27
#define TOKEN_DISP   28
#define TOKEN_DECAL  29
class MeshLoader
{
public:
	
	int GetTotalVertices()
	{
		return totalVetices;
	};
	
	typedef struct Indices
	{
		int vertexId;		
		int textureId;
		int normalId;
	}Indices;
	typedef struct TriFace
	{
		Indices Id[3];
	}TriFace;
	typedef struct QuadFace
	{
		Indices Id[4];
	}QuadFace;
	typedef struct FaceGroup
	{
		std::string usemtl;
		std::vector<TriFace> TriFaces;
	}FaceGroup;
	MeshLoader();
	~MeshLoader();
	bool LoadMeshFromOBJ(char* filename, DirectDevice* device, GameModel* model);
	

private:
	int getToken(std::string& buffer);
	float getFloat(std::string& buffer, int &startIndex);
	Indices getIndices(std::string& buffer, int &startIndex);
	int totalVetices;
	std::string mtllib;
};

#endif

