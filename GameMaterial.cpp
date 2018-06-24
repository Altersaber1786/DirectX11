#include "GameMaterial.h"



GameMaterial::GameMaterial()
{
	windowSquare[0].position = XMFLOAT3(-1.0f, 1.0f, 1.0f);  // Top left.
	windowSquare[0].texcoord = XMFLOAT2(0.0f, 0.0f);

	windowSquare[1].position = XMFLOAT3(1.0f, -1.0f, 1.0f);  // Bottom right.
	windowSquare[1].texcoord = XMFLOAT2(1.0f, 1.0f);

	windowSquare[2].position = XMFLOAT3(-1.0f, -1.0f, 1.0f);  // Bottom left.
	windowSquare[2].texcoord = XMFLOAT2(0.0f, 1.0f);

	windowSquare[3].position = XMFLOAT3(-1.0f, 1.0f, 1.0f);  // Top left.
	windowSquare[3].texcoord = XMFLOAT2(0.0f, 0.0f);

	windowSquare[4].position = XMFLOAT3(1.0f, 1.0f, 1.0f);  // Top right
	windowSquare[4].texcoord = XMFLOAT2(1.0f, 0.0f);

	windowSquare[5].position = XMFLOAT3(1.0f, -1.0f, 1.0f);  // Bottom right.
	windowSquare[5].texcoord = XMFLOAT2(1.0f, 1.0f);
}

bool GameMaterial::Initialize(DirectDevice*device)
{
	ID3DBlob* vsBytecode;
	ID3DBlob* psBytecode;
	
	if (!device->CompileD3DShader(L"TextureCombine.hlsl", "VS_MAIN", "vs_5_0", &vsBytecode))
	{
		MessageBox(NULL, "Failed to compile texture combining vertex shader", "Failed to compile shader code", NULL);
		return false;
	}
	if (FAILED(device->d3dDevice_->CreateVertexShader(vsBytecode->GetBufferPointer(), vsBytecode->GetBufferSize(), NULL, &vertexShader)))
	{
		MessageBox(NULL, "Failed to create game material vertex shader", "Failed to create shader", NULL);
		vsBytecode->Release();
		return false;
	}
	
	if (!device->CompileD3DShader(L"TextureCombine.hlsl", "PS_AMB", "ps_5_0", &psBytecode))
	{
		MessageBox(NULL, "Failed to compile texture combining amb pixel shader", "Failed to compile shader code", NULL);
		return false;
	}
	if (FAILED(device->d3dDevice_->CreatePixelShader(psBytecode->GetBufferPointer(), psBytecode->GetBufferSize(), NULL, &ambPixelShader)))
	{
		MessageBox(NULL, "Failed to create game material amb pixel shader", "Failed to create shader", NULL);
		psBytecode->Release();
		return false;
	}
	psBytecode->Release();
	if (!device->CompileD3DShader(L"TextureCombine.hlsl", "PS_DIFF", "ps_5_0", &psBytecode))
	{
		MessageBox(NULL, "Failed to compile texture combining diff pixel shader", "Failed to compile shader code", NULL);
		return false;
	}
	if (FAILED(device->d3dDevice_->CreatePixelShader(psBytecode->GetBufferPointer(), psBytecode->GetBufferSize(), NULL, &diffPixelShader)))
	{
		MessageBox(NULL, "Failed to create game material diff pixel shader", "Failed to create shader", NULL);
		psBytecode->Release();
		return false;
	}
	psBytecode->Release();
	if (!device->CompileD3DShader(L"TextureCombine.hlsl", "PS_SPEC", "ps_5_0", &psBytecode))
	{
		MessageBox(NULL, "Failed to compile texture combining spec pixel shader", "Failed to compile shader code", NULL);
		return false;
	}
	if (FAILED(device->d3dDevice_->CreatePixelShader(psBytecode->GetBufferPointer(), psBytecode->GetBufferSize(), NULL, &specPixelShader)))
	{
		MessageBox(NULL, "Failed to create game material spec pixel shader", "Failed to create shader", NULL);
		psBytecode->Release();
		return false;
	}
	psBytecode->Release();

	D3D11_INPUT_ELEMENT_DESC inputDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	if (FAILED(device->d3dDevice_->CreateInputLayout(inputDesc, 2, vsBytecode->GetBufferPointer(), vsBytecode->GetBufferSize(), &inputLayout)))
	{
		MessageBox(NULL, "Failed to create game material inputlayout", "Error", NULL);
		vsBytecode->Release();
		return false;
	};
	vsBytecode->Release();
	
	//Discribe and create Sampler state;
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if (FAILED(device->d3dDevice_->CreateSamplerState(&samplerDesc, &samplerState_)))
	{
		MessageBox(NULL, "Failed to create sampler state", NULL, NULL);
		return false;
	}
	////
	D3D11_BUFFER_DESC squareDesc;
	ZeroMemory(&squareDesc, sizeof(squareDesc));
	squareDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	squareDesc.ByteWidth = sizeof(windowSquare);
	squareDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA squareSubResource;
	ZeroMemory(&squareSubResource, sizeof(squareSubResource));
	squareSubResource.pSysMem = windowSquare;

	if (FAILED(device->d3dDevice_->CreateBuffer(&squareDesc, &squareSubResource, &squareVertexBuffer)))
	{
		MessageBox(0, "Failed to create material square vertex buffer", 0, 0);
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
		MessageBox(NULL, "Failed to create material properties buffer", NULL, NULL);
		return false;
	}

	return Initialized = true;
}

bool GameMaterial::CombineTexture(DirectDevice* device, TEXTURE_MAP_TYPE mapType, UINT width, UINT height)
{
	if (Initialized == false)
	{
		MessageBox(NULL, "Game material is not initialized", "Error", NULL);
		return false;
	}

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));

	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

       if (FAILED(device->d3dDevice_->CreateTexture2D(&texDesc, NULL, &tex2D_[mapType])))
		{
			MessageBox(NULL, "Cannot create material texture 2D", NULL, 0);
			return false;
		}
	   if(mapType == NORMAL_MAP)
	 if (FAILED(device->swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&tex2D_[mapType])))
	  {
		  MessageBox(NULL, "Failed to get the swap chain back buffer!", NULL, NULL);
		  return false;
	 };
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	if (FAILED(device->d3dDevice_->CreateRenderTargetView(tex2D_[mapType], &rtvDesc, &renderTargetView_[mapType])))
	{
		MessageBox(NULL, "Cannot create material render target view", NULL, 0);
		return false;
	}
	D3D11_VIEWPORT viewport;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	
	device->d3dContext_->RSSetViewports(1, &viewport);
	//Draw
	device->d3dContext_->IASetInputLayout(inputLayout);

	device->d3dContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	device->d3dContext_->IASetVertexBuffers(0, 1, &squareVertexBuffer, &stride, &offset);
	device->d3dContext_->VSSetShader(vertexShader, 0, 0);
	if(mapType == AMBIENT_MAP)
	device->d3dContext_->PSSetShader(ambPixelShader, 0, 0);
	else if(mapType == DIFFUSE_MAP)
	device->d3dContext_->PSSetShader(diffPixelShader, 0, 0);
	else
	device->d3dContext_->PSSetShader(specPixelShader, 0, 0);
	device->d3dContext_->PSSetShaderResources(0, 1, &shaderResourceView_[mapType]);
	device->d3dContext_->PSSetSamplers(0, 1, &samplerState_);
	device->d3dContext_->UpdateSubresource(propCB_, 0, 0, &surfaceProperties, 0, 0);
	device->d3dContext_->PSSetConstantBuffers(0, 1, &propCB_);
	device->d3dContext_->OMSetRenderTargets(1, &renderTargetView_[mapType], NULL);
	device->d3dContext_->OMSetRenderTargets(1, &renderTargetView_[mapType], NULL);
	device->d3dContext_->ClearRenderTargetView(renderTargetView_[mapType], clearColor);
	device->d3dContext_->Draw(6, 0);
	if (mapType == NORMAL_MAP)
	device->swapChain_->Present(0 ,0);

	device->d3dContext_->OMSetRenderTargets(NULL, NULL, NULL);
	shaderResourceView_[mapType]->Release();

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = -1;
	srvDesc.Texture2D.MostDetailedMip = 0;


	if (FAILED(device->d3dDevice_->CreateShaderResourceView(tex2D_[mapType], &srvDesc, &shaderResourceView_[mapType])))
	{
		MessageBox(NULL, "Failed to create material shader Resouce view", NULL, NULL);
		return false;
	}
//	tex2D_[mapType]->Release();

	if (mapType == AMBIENT_MAP)
		ambPixelShader->Release();
	else if (mapType == DIFFUSE_MAP)
		diffPixelShader->Release();
	else
		specPixelShader->Release();
	return true;
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
			if (texFile[i + 1] == L't'&&texFile[i + 2] == L'g'&&texFile[i + 3] == L'a')
				return EXTENSION_TGA;
			return EXTENSION_INVALID;
		}
		i++;

	}
	return EXTENSION_INVALID;
};

bool GameMaterial::LoadTextureFromFileAndCombine(DirectDevice* device, std::string textureFile, TEXTURE_MAP_TYPE mapType)
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
	else if (getExtension(widestr.c_str()) == EXTENSION_JPG|| getExtension(widestr.c_str()) == EXTENSION_PNG|| getExtension(widestr.c_str()) == EXTENSION_TGA)
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
			std::string pref = "texconv.exe -f R8G8B8A8_UNORM ";
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
	D3D11_TEXTURE2D_DESC texDesc;
	tex2D->GetDesc(&texDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = -1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	if (FAILED(device->d3dDevice_->CreateShaderResourceView(tex2D, &srvDesc, &shaderResourceView_[mapType])))
	{
		MessageBox(NULL, "Failed to create shader Resouce view", NULL, NULL);
		return false;
	}
	
	if (mapType == NORMAL_MAP);
	else
	{
		if (!CombineTexture(device, mapType, texDesc.Width, texDesc.Height))
		{
			MessageBox(NULL, "Failed to combine texture", NULL, NULL);
			tex2D->Release();
			return false;
		}
		tex2D->Release();
	}
	
	return true;
}

void GameMaterial::ReleaseAfterload()
{
	for (int i = 0; i < MAX_TEXTURE_MAP; i++)
		if (renderTargetView_[i] != nullptr)	renderTargetView_[i]->Release();
	if (inputLayout != nullptr)
		inputLayout->Release();
	if (squareVertexBuffer != nullptr)
		squareVertexBuffer->Release();
	vertexShader->Release();
	samplerState_->Release();
	squareVertexBuffer->Release();
}

GameMaterial::~GameMaterial()
{
	for(int i = 0; i < MAX_TEXTURE_MAP; i++)
		if (shaderResourceView_[i] != nullptr)
		{
			shaderResourceView_[i]->Release();
			shaderResourceView_[i] = nullptr;
		}
	
	for (int i = 0; i < MAX_TEXTURE_MAP; i++)
		if (tex2D_[i] != nullptr)	tex2D_[i]->Release();
	
	if (propCB_) propCB_->Release();
}
