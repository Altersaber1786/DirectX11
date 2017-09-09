#include "Sprite.h"


Sprite::Sprite() :rotation_(0.0f, 0.0f, 0.0f), scale_(1.0f, 1.0f, 1.0f), position_(0.0f, 0.0f, 0.0f)
{ 
}

bool Sprite::Initialize(XMFLOAT3 Position, XMFLOAT3 Rotation, XMFLOAT3 Scale, DirectDevice* device, LPCWSTR fxFile, const wchar_t* texFile)
{
	if (device == NULL) return false;
	SetPosition(Position);
	SetRotation(Rotation);
	SetScale(Scale);
	if (!LoadGeometry(device, fxFile, texFile))
	{
		return false;
	};
	return true;
}

void Sprite::SetPosition(XMFLOAT3 position)
{
	position_ = position;
};

void Sprite::SetRotation(XMFLOAT3 rotation)
{
	rotation_ = rotation;
};

void Sprite::SetScale(XMFLOAT3 scale)
{
	scale_ = scale;
};

XMMATRIX Sprite::GetWorldMatrix()
{
	XMMATRIX	translation	=	XMMatrixTranslation(position_.x, position_.y, position_.z);
	XMMATRIX	rotation	=	XMMatrixRotationRollPitchYaw(rotation_.x, rotation_.y, rotation_.z);
	XMMATRIX	scale		=	XMMatrixScaling(scale_.x, scale_.y, scale_.z);
	
	return	rotation*scale*translation;
};

bool Sprite::LoadGeometry(DirectDevice* device, LPCWSTR fxFile, const wchar_t* texFile)
{
	// vsBuffer, use to store compiled vertex shader byte code
	ID3DBlob* vsBuffer = nullptr; 

	bool compileResult = device->CompileD3DShader(fxFile, "VS_Main", "vs_4_0", &vsBuffer);

	if (compileResult == false)
	{
		MessageBox(NULL,"Error compiling the vertex shader!",NULL, NULL);
		return false;
	}

	HRESULT d3dResult;

	//Crete the vertex shader
	d3dResult = device->d3dDevice_->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), 0, &vertexShader_);

	if (FAILED(d3dResult))
	{
		MessageBox(NULL, "Error creating the vertex shader!", NULL, NULL);

		if (vsBuffer)
			vsBuffer->Release();

		return false;
	}

	//Describe input for the start of pipeline
	//Each vertex has the following structure, in this case, each input vertex has 2 elements: Position & texture coordinates
	D3D11_INPUT_ELEMENT_DESC inputDescription[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	unsigned int totalElements = ARRAYSIZE(inputDescription);

	//Create the input layout
	d3dResult = device->d3dDevice_->CreateInputLayout(inputDescription, totalElements,
		vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &inputLayout_);

	vsBuffer->Release();

	if (FAILED(d3dResult))
	{
		MessageBox(NULL, "Error creating the input layout!", NULL, NULL);
		return false;
	}

	//psBuffer, used to store pixel shader byte code 

	ID3DBlob* psBuffer = 0;
	compileResult = device->CompileD3DShader(fxFile, "PS_Main", "ps_4_0", &psBuffer);

	if (compileResult == false)
	{
		MessageBox(NULL, "Error compiling pixel shader!", NULL, NULL);
		return false;
	}
	// Create the pixel shader
	d3dResult = device->d3dDevice_->CreatePixelShader(psBuffer->GetBufferPointer(),
		psBuffer->GetBufferSize(), 0, &pixelShader_);

	psBuffer->Release();

	if (FAILED(d3dResult))
	{
		MessageBox(NULL, "Error creating pixel shader", NULL, NULL);
		return false;
	}

	//Vertices of our sprite
	VertexPos vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
	};
	
	//vertexBuffer description
	D3D11_BUFFER_DESC vertexDesc;
	ZeroMemory(&vertexDesc, sizeof(vertexDesc));
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.ByteWidth = sizeof(vertices);

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(resourceData));
	resourceData.pSysMem = vertices;

	//Create vertex buffer with our verticies stored.
	d3dResult = device->d3dDevice_->CreateBuffer(&vertexDesc, &resourceData, &vertexBuffer_);

	if (FAILED(d3dResult))
	{
		MessageBox(NULL, "Failed to create the vertex buffer!", NULL, NULL);
		return false;
	}

	//Index buffer
	WORD indices[] =
	{
		3, 1, 0, 2, 1, 3,
		6, 4, 5, 7, 4, 6,
		11, 9, 8, 10, 9, 11,
		14, 12, 13, 15, 12, 14,
		19, 17, 16, 18, 17, 19,
		22, 20, 21, 23, 20, 22
	};

	D3D11_BUFFER_DESC indexDesc;
	ZeroMemory(&indexDesc, sizeof(indexDesc));
	indexDesc.Usage = D3D11_USAGE_DEFAULT;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.ByteWidth = sizeof(indices);
	indexDesc.CPUAccessFlags = 0;
	resourceData.pSysMem = indices;

	d3dResult = device->d3dDevice_->CreateBuffer(&indexDesc, &resourceData, &indexBuffer_);

	if (FAILED(d3dResult))
	{
		MessageBox(NULL, "Failed to create the index buffer", NULL, NULL);
		return false;
	}

	//Create the view to look at shader's resource
	//In this section we set image load info = null, thread pump = null
	d3dResult = CreateDDSTextureFromFile(device->d3dDevice_, texFile, NULL, &shaderResourceView_);

	if (FAILED(d3dResult))
	{
		char extension[4] = {};
		char* pref = "texconv.exe texconv -f R8G8B8A8_UNORM ";
		char suff[32];
		unsigned long long num;
		wcstombs_s(&num, suff, 32, texFile, 512);
		char command[100] = {};
		strcat(command, pref);
		strcat(command, suff);
		system(command);
		if (FAILED(CreateDDSTextureFromFile(device->d3dDevice_, texFile, NULL, &shaderResourceView_)))
		{
			MessageBox(NULL, "Failed to load resource file, make sure the file extension is .DDS", NULL, NULL);
			return false;
		}
	}

	//Discribe and create Sampler state;
	D3D11_SAMPLER_DESC stateDescription;
	ZeroMemory(&stateDescription, sizeof(stateDescription));
	stateDescription.AddressU		=	D3D11_TEXTURE_ADDRESS_WRAP;
	stateDescription.AddressV		=	D3D11_TEXTURE_ADDRESS_WRAP;
	stateDescription.AddressW		=	D3D11_TEXTURE_ADDRESS_WRAP;
	stateDescription.Filter			=	D3D11_FILTER_MIN_MAG_MIP_LINEAR;	//Minification, magnification and mip maps created use linear interpolation
	stateDescription.MaxLOD			=	D3D11_FLOAT32_MAX;					//Max level of detail
	stateDescription.ComparisonFunc =	D3D11_COMPARISON_NEVER;				//Compare the sampled data with existing sampled data, in this case, never pass the comparison

	d3dResult = device->d3dDevice_->CreateSamplerState(&stateDescription, &samplerState_);

	if (FAILED(d3dResult))
	{
		MessageBox(NULL, "Failed to create sampler state", NULL, NULL);
		return false;
	}

	//Constant buffers to store matricies
	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = sizeof(XMMATRIX);
	constDesc.Usage = D3D11_USAGE_DEFAULT;

	//world matrix
	d3dResult = device->d3dDevice_->CreateBuffer(&constDesc, 0, &worldCB_);

	if (FAILED(d3dResult))
	{
		return false;
	}

	return true;
}

void Sprite::Update()
{
	worldMat_ = GetWorldMatrix();
	worldMat_ = XMMatrixTranspose(worldMat_);
}

void Sprite::Render(DirectDevice* device)
{
	
	
	unsigned int stride = sizeof(VertexPos);
	unsigned int offset = 0;

	//Set things appropriately to prepare the pipeline functions.
	// For input Assembler
	device->d3dContext_->IASetInputLayout(inputLayout_);
	device->d3dContext_->IASetVertexBuffers(0, 1, &vertexBuffer_, &stride, &offset);
	device->d3dContext_->IASetIndexBuffer(indexBuffer_, DXGI_FORMAT_R16_UINT, 0);
	device->d3dContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//Vertex shader
	device->d3dContext_->VSSetShader(vertexShader_, 0, 0);
	//Pixel shader
	device->d3dContext_->PSSetShader(pixelShader_, 0, 0);
	device->d3dContext_->PSSetShaderResources(0, 1, &shaderResourceView_); //Bind resource to pixel shader via shader Resource view
	device->d3dContext_->PSSetSamplers(0, 1, &samplerState_);			  //Set sampler state array, in this case, has 1 sampler

	//Update sub resources
	device->d3dContext_->UpdateSubresource(worldCB_, 0, 0, &worldMat_, 0, 0);
	
	device->d3dContext_->VSSetConstantBuffers(0, 1, &worldCB_);
	device->d3dContext_->DrawIndexed(36, 0, 0);
	
	return;
}

void Sprite::Relase()
{
	if (inputLayout_)			inputLayout_->			Release();
	if (vertexShader_)			vertexShader_->			Release();
	if (pixelShader_)			pixelShader_->			Release();
	if (shaderResourceView_)	shaderResourceView_->	Release();
	if (samplerState_)			samplerState_->			Release();
	if (vertexBuffer_)			vertexBuffer_->			Release();
	if (indexBuffer_)			indexBuffer_->			Release();
	if (worldCB_)				worldCB_->				Release();
	
};

Sprite::~Sprite()
{
	this->Relase();
}
