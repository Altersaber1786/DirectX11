#include "MeshLoader.h"
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
MeshLoader::MeshLoader()
{
	totalVetices = 0;
}

bool inline isFloat(char* myString)
{
	std::istringstream iss(myString);
	float f;
	iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
	 // Check the entire string was consumed and if either failbit or badbit is set
	return iss.eof() && !iss.fail();
}

float getFloat(char* buffer, int& startIndex, int& endIndex)
{
	
	int count = endIndex - startIndex + 1;
	char* temp = new char[count];
	for (int i = 0; i < count - 1; i++)
	{
		temp[i] = buffer[startIndex];
		startIndex++;
	}
	temp[count - 1] = '\0';
	if (!isFloat(temp))
	{
		std::string message = std::to_string(startIndex);
		MessageBox(NULL, (LPCSTR)&message, "Invalid float value", NULL);
		return 0;
	}
	startIndex++;
	endIndex = startIndex + 1;
	while (buffer[endIndex] != ' '&&buffer[endIndex] != '\n'&&buffer[endIndex] != '\0')
	{
		endIndex++;
	}
	return static_cast<float> (atof(temp));
}

MeshLoader::Indices getIndices(char* buffer, int& startIndex, int& endIndex)
{
	MeshLoader::Indices index;
	index.vertexId = 0;
	index.textureId = 0;
	index.normalId = 0;
	

	int count = endIndex - startIndex;
	char* temp = new char[count];
	for (int i = 0; i < count ; i++)
	{
		temp[i] = buffer[startIndex];
		startIndex++;
	}
	startIndex++;
	endIndex = startIndex + 1;
	while (buffer[endIndex] != ' '&&buffer[endIndex] != '\n'&&buffer[endIndex] != '\0')
	{
		endIndex++;
	}
	int i = 0;
	while (temp[i] != '/')
	{
		index.vertexId = index.vertexId*10 + temp[i] - '0';
		i++;
	}
	i++;
	while (temp[i] != '/')
	{
		index.textureId = index.textureId * 10 + temp[i] - '0';
		i++;
	}
	i++;
	while (i < count)
	{
		index.normalId = index.normalId * 10 + temp[i] - '0';
		i++;
	}
	
	return index;
}

int MeshLoader::getToken(char* buffer, int& startIndex, int& endIndex)
{
	if (buffer[startIndex] == '\n')
	{
		startIndex++;
		return TOKEN_EMPTY_LINE;
	}
	int count = endIndex - startIndex;
	char* temp = new char[count];
	for (int i = 0; i < count; i++)
	{
		temp[i] = buffer[startIndex];
		startIndex++;
	}

	startIndex++;
	endIndex = startIndex + 1;
	char c = buffer[startIndex];
	while (buffer[endIndex] != ' '&&buffer[endIndex] != '\n'&&buffer[endIndex] != '\0')
	{
		endIndex++;
	}
	if (temp[0] == '#')
	{
		
		delete []temp;
		return TOKEN_HASH;
	}

	if (temp[0] == 'v')
	{
		if (count == 1)
		{
			delete []temp;
			return TOKEN_V;
		}
		if (temp[1] == 't')
		{
			delete []temp;
			return TOKEN_VT;
		}
		if (temp[1] == 'n')
		{
			delete[] temp;
			return TOKEN_VN;
		}
	}
	if (temp[0] == 'f')
	{
		delete[] temp;
		return TOKEN_F;
	}
	
	if (temp[0] == 'g')
	{
		delete[] temp;
		return TOKEN_G;
	}
	if (temp[0] == 's')
	{
		delete[] temp;
		return TOKEN_S;
	}
	if (count == 6)
	{
		int x = 0;
		for (int i = 0; i < count; i++)
		{
			if (temp[i] == mtllib[i]) x++;
		}
		if(x==count)
		{
		delete[] temp;
		return TOKEN_MTLLIB;
	    }
		x = 0;
		for (int i = 0; i < count; i++)
		{
			if (temp[i] == usemtl[i]) x++;
		}
		if (x == count)
		{
			delete[] temp;
			return TOKEN_USEMTL;
		}
	}

	delete[] temp;	
	return TOKEN_INVALID;
}

bool MeshLoader::LoadMeshFromOBJ(char* filename, DirectDevice* device, ID3D11Buffer** vertexBuffer)
{
	std::vector <XMFLOAT3> vertices;
	std::vector <XMFLOAT3> normals;
	std::vector <XMFLOAT2> texcoords;
	std::vector <Face> faces;
	int startIndex =0,  endIndex = 1;
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

	char *buffer = new char[fileSize];
	if (buffer == 0) 
		return false;
	memset(buffer, '\0', fileSize);

	fileStream.read(buffer, fileSize);

	while (endIndex < fileSize)
	{
		
		if (buffer[endIndex] == ' ')
		{

			switch (getToken(buffer, startIndex, endIndex))
			{
			case TOKEN_V:
			  {
				XMFLOAT3 temp;
				temp.x = getFloat(buffer, startIndex, endIndex);
				temp.y = getFloat(buffer, startIndex, endIndex);
				temp.z = getFloat(buffer, startIndex, endIndex);
				vertices.push_back(temp);
				break;
			  }
			case TOKEN_VN:
			  {
				XMFLOAT3 temp;
				temp.x = getFloat(buffer, startIndex, endIndex);
				temp.y = getFloat(buffer, startIndex, endIndex);
				temp.z = getFloat(buffer, startIndex, endIndex);
				normals.push_back(temp);
				break;
			  }
			case TOKEN_VT:
			  {
				XMFLOAT2 temp;
				temp.x = getFloat(buffer, startIndex, endIndex);
				temp.y = getFloat(buffer, startIndex, endIndex);
				texcoords.push_back(temp);
				break;
			  }
			case TOKEN_F:
			  {
				Face face;
				face.Id[0] = getIndices(buffer, startIndex, endIndex);
				face.Id[1] = getIndices(buffer, startIndex, endIndex);
				face.Id[2] = getIndices(buffer, startIndex, endIndex);
				faces.push_back(face);
				break;
			  }
			  
			case TOKEN_INVALID:
			  {
				std::string message = std::to_string(startIndex);
				MessageBox(NULL, (LPCSTR)&message, "Invalid symbol", NULL);
				return false;
			  }
			
			case TOKEN_EMPTY_LINE:
				break;
			default:
			case TOKEN_HASH:
			  {
				while (buffer[endIndex] != '\n' && buffer[endIndex] != '\0') 
				{
					endIndex++;
				}
				startIndex = endIndex + 1;
				endIndex = startIndex + 1;
				
				char awd = buffer[endIndex];
				int qwe = 1;
				break;
			  }
			}; 
		}
		else endIndex++;
	}


	//Adding final vertices to an array
	int numFaces = faces.size();
	totalVetices = numFaces * 3;
	Vertex* m_vertices = new Vertex[totalVetices];
	int k = 0;
	for (int i = 0; i < numFaces; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			m_vertices[k].position = vertices[faces[i].Id[j].vertexId - 1];
			m_vertices[k].texcoord = texcoords[faces[i].Id[j].textureId - 1];
			m_vertices[k].normal = normals[faces[i].Id[j].normalId - 1];
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
