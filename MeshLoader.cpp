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
	return atof(buffer.c_str() + startIndex);
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
	return TOKEN_INVALID;
}

bool MeshLoader::LoadMeshFromOBJ(char* filename, DirectDevice* device, ID3D11Buffer** vertexBuffer)
{
	std::vector <XMFLOAT3> vertices;
	std::vector <XMFLOAT3> normals;
	std::vector <XMFLOAT2> texcoords;
	std::vector <TriFace> Trifaces;
	int startIndex = 0;
	int fileSize = 0;

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
				temp.x = getFloat(buffer, startIndex);
				while (buffer[startIndex] != 32)startIndex++;
				temp.y = getFloat(buffer, startIndex);
				texcoords.push_back(temp);
				break;
			}
			case TOKEN_F3:
			{
				TriFace face;
				while (buffer[startIndex] != 32)startIndex++;
				face.Id[0] = getIndices(buffer, startIndex);
				while (buffer[startIndex] != 32)startIndex++;
				face.Id[1] = getIndices(buffer, startIndex);
				while (buffer[startIndex] != 32)startIndex++;
				face.Id[2] = getIndices(buffer, startIndex);
				Trifaces.push_back(face);
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
				Trifaces.push_back(face1);
				Trifaces.push_back(face2);
				break;
			}
			case TOKEN_INVALID:
			{
				std::string message = "Invalid token at line " + std::to_string(lineCount);
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

		


	//Adding final vertices to an array
	int numFaces = static_cast<int>(Trifaces.size());
	totalVetices = numFaces * 3;
	if (texcoords.size() == 0) texcoords.push_back(XMFLOAT2(0.0f, 0.0f));
	if (normals.size() == 0)normals.push_back(XMFLOAT3(0.0f, 0.0f, 0.0f));

	Vertex* m_vertices = new Vertex[totalVetices];
	int k = 0;
	for (int i = 0; i < numFaces; i++)
	{
		if (i == 54)
		{
			int asdqwd = 0;
		}
		for (int j = 0; j < 3; j++)
		{
			

			m_vertices[k].position = vertices[Trifaces[i].Id[j].vertexId - 1];
			m_vertices[k].texcoord = texcoords[Trifaces[i].Id[j].textureId - 1];
			m_vertices[k].normal = normals[Trifaces[i].Id[j].normalId - 1];
			k++;
		}
	}
	

	//vertexBuffer description
	D3D11_BUFFER_DESC vertexDesc;
	ZeroMemory(&vertexDesc, sizeof(vertexDesc));
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.ByteWidth = sizeof(Vertex)*totalVetices;

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(resourceData));
	resourceData.pSysMem = m_vertices;

	//Create vertex buffer with our verticies.
	if (FAILED(device->d3dDevice_->CreateBuffer(&vertexDesc, &resourceData, vertexBuffer)))
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
