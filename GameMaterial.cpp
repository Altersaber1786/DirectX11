#include "GameMaterial.h"



GameMaterial::GameMaterial()
{

}



GameMaterial::FILE_EXTENSION GameMaterial::getExtension(std::wstring texFile)
{
	int i = 1;
	while (i < 256)
	{
		if (texFile[i] == L'.')
		{
			if (texFile[i + 1] == L'D'&&texFile[i + 2] == L'D'&&texFile[i + 3] == L'S')
				return EXTENSION_DDS;
			if (texFile[i + 1] == L'j'&&texFile[i + 2] == L'p'&&texFile[i + 3] == L'g')
				return EXTENSION_JPG;
			if (texFile[i + 1] == L'p'&&texFile[i + 2] == L'n'&&texFile[i + 3] == L'g')
				return EXTENSION_PNG;
			return EXTENSION_INVALID;
		}
		i++;

	}
	return EXTENSION_INVALID;
};

bool GameMaterial::LoadContent(DirectDevice* device, std::string textureFile)
{
	std::wstring widestr = std::wstring(textureFile.begin(), textureFile.end());
	HRESULT d3dResult;
	ID3D11Texture2D* tex2D;
	if (getExtension(widestr) == EXTENSION_DDS)
	{
		d3dResult = CreateDDSTextureFromFile(device->d3dDevice_, widestr.c_str(), (ID3D11Resource**)&tex2D, NULL);
		if (FAILED(d3dResult))
		{
				MessageBox(NULL, "Failed to load resource file, make sure the file extension is .DDS", NULL, NULL);
				return false;
		}
	}
	else if (getExtension(widestr.c_str()) == EXTENSION_JPG|| getExtension(widestr.c_str()) == EXTENSION_PNG)
	{
		////////
		std::string ddsTemp = textureFile;
		UINT i = 1;
		while (ddsTemp[i] != '.')
		{
			i++;
		}
		ddsTemp[i + 1] = 'D';
		ddsTemp[i + 2] = 'D';
		ddsTemp[i + 3] = 'S';
		struct stat buffer; // check if the DDS file exist
		if (stat(ddsTemp.c_str(), &buffer) == 0) {
			std::wstring wideDDS = std::wstring(ddsTemp.begin(), ddsTemp.end());
			d3dResult = CreateDDSTextureFromFile(device->d3dDevice_, wideDDS.c_str(), (ID3D11Resource**)&tex2D, NULL);
			if (FAILED(d3dResult))
			{
				MessageBox(NULL, "Failed to load resource file, make sure the file extension is .DDS", NULL, NULL);
				return false;
			}
		}
		else   //If failed to find dds file, convert the jpg to dds
		{
			std::string pref = "texconv.exe texconv -f R8G8B8A8_UNORM_SRGB ";
			std::string command = pref + textureFile;
			system(command.c_str());
			//Then load the DDS 
			std::wstring wideDDS = std::wstring(ddsTemp.begin(), ddsTemp.end());
			d3dResult = CreateDDSTextureFromFile(device->d3dDevice_, wideDDS.c_str(), (ID3D11Resource**)&tex2D, NULL);
			if (FAILED(d3dResult))
			{
				MessageBox(NULL, "Failed to load resource file, make sure the image exist", NULL, NULL);
				return false;
			}
		}
	}
	else
		return false;
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


GameMaterial::~GameMaterial()
{
	if (shaderResourceView_)	shaderResourceView_->Release();
	if (propCB_) propCB_->Release();
}
