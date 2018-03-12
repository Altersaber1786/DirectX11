#include "GameMaterial.h"



GameMaterial::GameMaterial()
{
	totalObjects = 0;
}

bool GameMaterial::pushObject(GameObject* obj)
{
	ObjectList.push_back (obj);
	totalObjects = static_cast<unsigned int>(ObjectList.size());
	return true;
};

GameMaterial::FILE_EXTENSION GameMaterial::getExtension(wchar_t* texFile)
{
	int i = 0;
	while (i < 256)
	{
		if (texFile[i] == L'.')
		{
			if (texFile[i + 1] == L'D'&&texFile[i + 2] == L'D'&&texFile[i + 3] == L'S')
				return EXTENSION_DDS;
			if (texFile[i + 1] == L'J'&&texFile[i + 2] == L'P'&&texFile[i + 3] == L'G')
				return EXTENSION_JPG;
			if (texFile[i + 1] == L'P'&&texFile[i + 2] == L'N'&&texFile[i + 3] == L'G')
				return EXTENSION_PNG;
			return EXTENSION_INVALID;
		}
		i++;

	}
	return EXTENSION_INVALID;
};

bool GameMaterial::LoadContent(DirectDevice* device, wchar_t* textureFile)
{
	
	HRESULT d3dResult;
	ID3D11Texture2D* tex2D;
	d3dResult = CreateDDSTextureFromFile(device->d3dDevice_, textureFile, (ID3D11Resource**)&tex2D, NULL);

	if (FAILED(d3dResult))
	{
		char extension[4] = {};
		char* pref = "texconv.exe texconv -f R8G8B8A8_UNORM_SRGB ";
		char suff[32];
		unsigned long long num;
		wcstombs_s(&num, suff, 32, textureFile, 512);
		char command[256] = {};
		strcat(command, pref);
		strcat(command, suff);
		system(command);
		if (FAILED(CreateDDSTextureFromFile(device->d3dDevice_, textureFile, (ID3D11Resource**)&tex2D, NULL)))
		{
			MessageBox(NULL, "Failed to load resource file, make sure the file extension is .DDS", NULL, NULL);
			return false;
		}
	}
	D3D11_TEXTURE2D_DESC tex2Ddesc;
	tex2D->GetDesc(&tex2Ddesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = -1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	if (FAILED(device->d3dDevice_->CreateShaderResourceView(tex2D, &srvDesc, &shaderResourceView_)))
	{
		MessageBox(NULL, "Failed to create shader Resouce view", NULL, NULL);
		return false;
	};
	//Discribe and create Sampler state;
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	d3dResult = device->d3dDevice_->CreateSamplerState(&samplerDesc, &samplerState_);

	if (FAILED(d3dResult))
	{
		MessageBox(NULL, "Failed to create sampler state", NULL, NULL);
		return false;
	}
	
	//Constant buffers description
	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = sizeof(MaterialProperties);
	constDesc.Usage = D3D11_USAGE_DEFAULT;

	if (FAILED(device->d3dDevice_->CreateBuffer(&constDesc, 0, &propCB_)))
	{
		MessageBox(NULL, "Failed to create material diffuse buffer", NULL, NULL);
		return false;
	}
	
	return true;
}

void GameMaterial::Update()
{
	
	for (UINT i = 0; i < totalObjects; i++)
	{
		ObjectList[i]->Update();
	}
	
}

void GameMaterial::Render(DirectDevice* device, XMMATRIX viewMat, XMMATRIX projMat)
{

	device->d3dContext_->PSSetShaderResources(0, 1, &shaderResourceView_); //Bind resource to pixel shader via shader Resource view
	device->d3dContext_->PSSetSamplers(0, 1, &samplerState_);
	device->d3dContext_->UpdateSubresource(propCB_, 0, 0, &surfaceProperties, 0, 0);
	device->d3dContext_->PSSetConstantBuffers(1, 1, &propCB_);
	
	for (UINT i = 0; i < totalObjects; i++)
	{
		ObjectList[i]->Render(device, viewMat, projMat);
	}
};

GameMaterial::~GameMaterial()
{
	if (inputLayout_)			inputLayout_->Release();
	if (vertexShader_)			vertexShader_->Release();
	if (pixelShader_)			pixelShader_->Release();
	if (shaderResourceView_)	shaderResourceView_->Release();
	if (samplerState_)			samplerState_->Release();

}
