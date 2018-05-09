#include "MeshLoader.h"
#include <fstream>
#include <vector>
#include <string>
#include <sstream>


MeshLoader::MeshLoader()
{
	totalVetices = 0;
}


float MeshLoader::getFloat(std::string &buffer, int &startIndex)
{
	while (buffer[startIndex] == 32)
		startIndex++;
	return static_cast<float>(atof(buffer.c_str() + startIndex));
}

MeshLoader::Indices MeshLoader::getIndices(std::string &buffer, int &startIndex)
{
	while (buffer[startIndex] == 32)
		startIndex++;
	MeshLoader::Indices index;
	index.vertexId = atoi(buffer.c_str() + startIndex);
	while (buffer[startIndex] != '/')
		startIndex++;
	startIndex++;
	if (buffer[startIndex] == '/')
	{
		index.textureId = 1;
	}
	else
		index.textureId = atoi(buffer.c_str() + startIndex);

	while (buffer[startIndex] != '/')
		startIndex++;
	startIndex++;
	if (buffer[startIndex] == 32 || buffer[startIndex] == '\0')
	{
		index.normalId = 1;
	}
	else
	index.normalId = atoi(buffer.c_str() + startIndex);
	
	return index;
}

int MeshLoader::getToken(std::string& buffer)
{
	if (buffer[0] == '\0')
	{
		return TOKEN_EMPTY_LINE;
	}
	
	if (buffer[0] == '#')
	{
		return TOKEN_HASH;
	}
	if (buffer[0] == 'K')
	{
		if (buffer[1] == 'd')
			return TOKEN_KD;
		if (buffer[1] == 'a')
			return TOKEN_KA;
		if (buffer[1] == 's')
			return TOKEN_KS;
	}
	if (buffer[0] == 'N')
	{
		if (buffer[1] == 'i')
			return TOKEN_NI;
		if (buffer[1] == 's')
			return TOKEN_NS;
	}
	if (buffer[0] == 'n')
	{
		if (buffer[1] == 'e')
			if (buffer[2] == 'w')
				return TOKEN_NEWMTL;
	}
	if (buffer[0] == 'm')
	{
		if (buffer[1] == 'a')
			if (buffer[2] == 'p')
				if (buffer[5] == 'd')
					return TOKEN_MAP_KD;
				else if (buffer[5] == 'a')
					return TOKEN_MAP_KA;
				else if (buffer[5] == 's')
					return TOKEN_MAP_KS;
	}
	if (buffer[0] == 'r')
	{
		if (buffer[1] == 'e')
			if (buffer[2] == 'f')
				if (buffer[3] == 'l')
					return TOKEN_REFL;
				
	}
	if (buffer[0] == 'b')
	{
		if (buffer[1] == 'u')
			if (buffer[2] == 'm')
				if (buffer[3] == 'p')
					return TOKEN_BUMP;
	}
	
	if (buffer[0] == 'd')
	{
			return TOKEN_D;
	}
	if (buffer[0] == 'T')
	{
		if (buffer[1] == 'f')
			return TOKEN_TF;
	}
	if (buffer[0] == 'i')
	{
		if (buffer[1] == 'l')
			return TOKEN_ILLUM;
	}
	if (buffer[0] == 'v')
	{
		if (buffer[1] == ' ')
		{
			return TOKEN_V;
		}
		if (buffer[1] == 't')
		{
			return TOKEN_VT;
		}
		if (buffer[1] == 'n')
		{
			return TOKEN_VN;
		}
		return TOKEN_INVALID;
	}
	if (buffer[0] == 'f')
	{
		int i = 0;
		int faceCount = 0;
		while (buffer[i] != '\0')
		{
			if (buffer[i] == '/')
			{
				faceCount++;
			}
			i++;
		}
		if(faceCount == 6)
		{ 
			return TOKEN_F3;
		}
		else if(faceCount == 8)
		{
			return TOKEN_F4;
		}
		else
		{
			return TOKEN_INVALID;
		}
	}
	
	if (buffer[0] == 'g')
	{
		int i = 1;
		while (buffer[i] == 32)i++;
		int v = 0x0;
		for (int j = 0; j < 7; j++)
		{
			if (buffer[i] == '\0')
				break;
			int x = buffer[i];
			v += x << j;
			i++;
		}
		if (v == 14238)
			return TOKEN_EMPTY_LINE;
		return TOKEN_G;
	}
	if (buffer[0] == 's')
	{
		return TOKEN_S;
	}
	if (buffer[0] == 'o')
	{
		return TOKEN_O;
	}
	if (buffer[1] != 32)
	{
		int v = 0x0;
		for (int i = 0; i < 6; i++)
		{
			int x = buffer[i];
			v += x << i;
		}
		if (v == 6453)
			return TOKEN_MTLLIB;
		if (v == 6935)
			return TOKEN_USEMTL;
	}
	return TOKEN_EMPTY_LINE;
}

bool MeshLoader::LoadMeshFromOBJ(char* filename, DirectDevice* device, GameModel* model)
{
	std::vector <XMFLOAT3> vertices;
	std::vector <XMFLOAT3> normals;
	std::vector <XMFLOAT2> texcoords;
	std::vector <FaceGroup> faceGroups;
	FaceGroup tempGroup;
	int startIndex = 0;
	int fileSize = 0;
	int numGroup = -1;
	std::ifstream fileStream;
	fileStream.open(filename, std::ifstream::in);

	if (fileStream.is_open() == false)
		return false;

	fileStream.seekg(0, std::ios::end);
	fileSize = (int)fileStream.tellg();
	fileStream.seekg(0, std::ios::beg);
	if (fileSize <= 0)
		return false;

	std::string buffer;

	UINT lineCount = 0;
	while (fileStream.good())
	{
		startIndex = 0;
		lineCount++;
		getline(fileStream, buffer);
		switch (getToken(buffer))
		{
		case TOKEN_V:
		{
			XMFLOAT3 temp;
			while (buffer[startIndex] != 32)startIndex++;
			temp.x = getFloat(buffer, startIndex);
			while (buffer[startIndex] != 32)startIndex++;
			temp.y = getFloat(buffer, startIndex);
			while (buffer[startIndex] != 32)startIndex++;
			temp.z = getFloat(buffer, startIndex);
			vertices.push_back(temp);
			break;
		}
		case TOKEN_VN:
		{
			XMFLOAT3 temp;
			while (buffer[startIndex] != 32)startIndex++;
			temp.x = getFloat(buffer, startIndex);
			while (buffer[startIndex] != 32)startIndex++;
			temp.y = getFloat(buffer, startIndex);
			while (buffer[startIndex] != 32)startIndex++;
			temp.z = getFloat(buffer, startIndex);
			normals.push_back(temp);
			break;
		}
		case TOKEN_VT:
		{
			XMFLOAT2 temp;
			while (buffer[startIndex] != 32)startIndex++;
			temp.x = 1.0f - getFloat(buffer, startIndex);
			while (buffer[startIndex] != 32)startIndex++;
			temp.y = 1.0f - getFloat(buffer, startIndex);
			texcoords.push_back(temp);
			break;
		}
		case TOKEN_G:
		{
			numGroup++;
			if (numGroup > faceGroups.size())
			{
				faceGroups.push_back(tempGroup);
				tempGroup.TriFaces.clear();
			}
			break;
		}
		case TOKEN_F3:
		{
			FaceGroup group;
			TriFace face;
			while (buffer[startIndex] != 32)startIndex++;
			face.Id[0] = getIndices(buffer, startIndex);
			while (buffer[startIndex] != 32)startIndex++;
			face.Id[1] = getIndices(buffer, startIndex);
			while (buffer[startIndex] != 32)startIndex++;
			face.Id[2] = getIndices(buffer, startIndex);
			tempGroup.TriFaces.push_back(face);
			break;
		}
		case TOKEN_F4:
		{
			QuadFace face;
			TriFace face1, face2;
			while (buffer[startIndex] != 32)startIndex++;
			face.Id[0] = getIndices(buffer, startIndex);
			while (buffer[startIndex] != 32)startIndex++;
			face.Id[1] = getIndices(buffer, startIndex);
			while (buffer[startIndex] != 32)startIndex++;
			face.Id[2] = getIndices(buffer, startIndex);
			while (buffer[startIndex] != 32)startIndex++;
			face.Id[3] = getIndices(buffer, startIndex);

			face1.Id[0] = face.Id[0];
			face1.Id[1] = face.Id[1];
			face1.Id[2] = face.Id[2];

			face2.Id[0] = face.Id[0];
			face2.Id[1] = face.Id[2];
			face2.Id[2] = face.Id[3];
			tempGroup.TriFaces.push_back(face1);
			tempGroup.TriFaces.push_back(face2);
			break;
		}
		case TOKEN_MTLLIB:
		{
			int i = 6;
			for (i; i < buffer.length(); i++)
			{
				if (buffer[i] != 32) break;
			}
			mtllib = buffer.substr(i, buffer.length() - i);
			break;
		}
		case TOKEN_USEMTL:
		{
			int i = 6;
			for (i; i < buffer.length(); i++)
			{
				if (buffer[i] != 32) break;
			}
			tempGroup.usemtl = buffer.substr(i, buffer.length() - i);
			break;
		}

		case TOKEN_INVALID:
		{
			std::string message = "Invalid token at line ";
			message = message +std::to_string(lineCount);
			MessageBox(NULL, (LPCSTR)&message, "Invalid symbol", NULL);
			return false;
		}
		case TOKEN_EMPTY_LINE:
		case TOKEN_HASH:
		default:
			break;
		};
	}
	fileStream.close();


	faceGroups.push_back(tempGroup);
	tempGroup.TriFaces.clear();
	int numface_total = 0;
	for (int z = 0; z < faceGroups.size(); z++)
	{
		numface_total += faceGroups[z].TriFaces.size();
	}
	totalVetices = numface_total * 3;
	GameModel::Vertex* m_vertices = new GameModel::Vertex[totalVetices];
	int k = 0;

	//Open the mtl file
	int asdwqd = mtllib.size();
	std::ifstream mtlStream;
	
	//Adding final vertices to an array
	for (int x = 0; x < faceGroups.size(); x++)
	{
		mtlStream.open("./ModelsandTextures/"+mtllib, std::ifstream::in);
		if (mtlStream.is_open() == false)
		{
			MessageBox(NULL, "Failed to open mtllib, make sure the file exist", "Failed to open file", NULL);
			return false;
		}
		int numFaces = static_cast<int>(faceGroups[x].TriFaces.size());
		if (texcoords.size() == 0) texcoords.push_back(XMFLOAT2(0.0f, 0.0f));
		if (normals.size() == 0)normals.push_back(XMFLOAT3(0.0f, 0.0f, 0.0f));
		Box tempBox;
		tempBox.StartVertex = k;
		GameMaterial* tempMat = new GameMaterial();
		tempMat->Initialize(device);
		while (mtlStream.good())
		{
			getline(mtlStream, buffer);
			if(getToken(buffer)==TOKEN_NEWMTL)
				if (strcmp(faceGroups[x].usemtl.c_str(), buffer.c_str() + 7) == 0)
				{
					while (mtlStream.good())
					{
						getline(mtlStream, buffer);
						if (getToken(buffer) == TOKEN_NEWMTL) break;
						switch (getToken(buffer))
						{
							case TOKEN_KA:
							{
								startIndex = 2;
								tempMat->surfaceProperties.Ka = getFloat(buffer, startIndex);
								break;
							}
							case TOKEN_KD:
							{
								startIndex = 2;
								tempMat->surfaceProperties.Kd = getFloat(buffer, startIndex);
								break;
							}
							case TOKEN_KS:
							{
								startIndex = 2;
								tempMat->surfaceProperties.Ks = getFloat(buffer, startIndex);
								break;
							}
							case TOKEN_NS:
							{
								startIndex = 2;
								tempMat->surfaceProperties.SpecExp = getFloat(buffer, startIndex);
								break;
							}
							case TOKEN_MAP_KD:
							{
								std::string str = std::string(buffer.begin() + 7, buffer.end());
								if (!tempMat->LoadTextureFromFileAndCombine(device, "./ModelsandTextures/"+str, GameMaterial::DIFFUSE_MAP))
								{
									MessageBox(NULL, "Invalid texture file, make sure the file exist.", "Invalid symbol", NULL);
									return false;
								}
								break;
							}
							case TOKEN_MAP_KA:
							{
								std::string str = std::string(buffer.begin() + 7, buffer.end());
								if (str.length() != 0)
								{
									if (!tempMat->LoadTextureFromFileAndCombine(device, "./ModelsandTextures/" + str, GameMaterial::AMBIENT_MAP))
									{
										MessageBox(NULL, "Invalid texture file, make sure the file exist.", "Invalid symbol", NULL);
										return false;
									}
								}
								break;
							}
							case TOKEN_MAP_KS:
							{
								std::string str = std::string(buffer.begin() + 7, buffer.end());
								if (str.length() != 0)
								{
									if (!tempMat->LoadTextureFromFileAndCombine(device, "./ModelsandTextures/" + str, GameMaterial::SPECULAR_MAP))
									{
										MessageBox(NULL, "Invalid texture file, make sure the file exist.", "Invalid symbol", NULL);
										return false;
									}
								}
								break;
							}
							case TOKEN_BUMP:
							{
								std::string str = std::string(buffer.begin() + 5, buffer.end());
								if (str.length() != 0)
								{
									if (!tempMat->LoadTextureFromFileAndCombine(device, "./ModelsandTextures/" + str, GameMaterial::NORMAL_MAP))
									{
										MessageBox(NULL, "Invalid texture file, make sure the file exist.", "Invalid symbol", NULL);
										return false;
									}
								}
								break;
							}
							case TOKEN_INVALID:
							{
								std::string message = "Invalid token";
								MessageBox(NULL, (LPCSTR)&message, "Invalid symbol", NULL);
								return false;
							}
							default:
								break;
						}
					}
					break;
				}
		}
		mtlStream.close();
		tempMat->ReleaseAfterload();
		tempBox.material = tempMat;

		for (int i = 0; i < numFaces; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				m_vertices[k].position = vertices[faceGroups[x].TriFaces[i].Id[j].vertexId - 1];
				m_vertices[k].texcoord = texcoords[faceGroups[x].TriFaces[i].Id[j].textureId - 1];
				m_vertices[k].normal = normals[faceGroups[x].TriFaces[i].Id[j].normalId - 1];
				k++;
			}
		}
		tempBox.NumVertex = k - tempBox.StartVertex;
		model->BoxList.push_back(tempBox);
	}
	
	//vertexBuffer description
	D3D11_BUFFER_DESC vertexDesc;
	ZeroMemory(&vertexDesc, sizeof(vertexDesc));
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.ByteWidth = sizeof(GameModel::Vertex)*totalVetices;

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(resourceData));
	resourceData.pSysMem = m_vertices;

	//Create vertex buffer with our verticies.
	if (FAILED(device->d3dDevice_->CreateBuffer(&vertexDesc, &resourceData, &model->vertexBuffer_)))
	{
		MessageBox(NULL, "Failed to create the vertex buffer!", NULL, NULL);
		return false;
	}

	delete m_vertices;

	return true;
}

MeshLoader::~MeshLoader()
{
}
