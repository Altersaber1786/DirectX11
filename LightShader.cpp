#include "LightShader.h"

LightShader::LightShader()
{
	m_totalLights = 0;
	LightSource light_1;
	light_1.position = XMFLOAT4(0.0f, 0.0f, 10.0f, 0.0f);
	light_1.intensity = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	m_lightSources.push_back(light_1);
	m_totalLights = static_cast<UINT>(m_lightSources.size());
}
bool LightShader::CompileD3DShader(LPCWSTR filePath, LPCSTR entry, LPCSTR shaderModel, ID3DBlob** buffer)
{	
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* errorBuffer = nullptr;
	HRESULT result;
	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		NULL, NULL
	};

	result = D3DCompileFromFile(filePath, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry, shaderModel, shaderFlags, 0, buffer, &errorBuffer);

	if (FAILED(result))
	{
		MessageBox(NULL,"Error compile shader code from file!", NULL, 0);
		if (errorBuffer != 0)
		{

			OutputDebugStringA((char*)errorBuffer->GetBufferPointer());
			errorBuffer->Release();
		}

		return false;
	}

	if (errorBuffer != 0)
		errorBuffer->Release();

	return true;
}
bool LightShader::Initialize(ID3D11Device* d3dDevice)
{
	HRESULT result;
	ID3DBlob* vsBytecode;
	ID3DBlob* psBytecode;

	bool compileResult = CompileD3DShader(L"PackingGBuffer.fx", "VS_Main", "vs_5_0", &vsBytecode);

	if (compileResult == false)
	{
		MessageBox(NULL, "Error compiling the packing g buffer vertex shader!", NULL, NULL);
		return false;
	}

	result = d3dDevice->CreateVertexShader(vsBytecode->GetBufferPointer(), vsBytecode->GetBufferSize(), 0, &m_gPackingVShader);
	if (FAILED(result))
	{
		MessageBox(NULL, "Error creating the model vertex shader!", NULL, NULL);

		if (vsBytecode)
			vsBytecode->Release();

		return false;
	}

	D3D11_INPUT_ELEMENT_DESC gpackingIputDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3D11_INPUT_ELEMENT_DESC deferredShadingIputDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	result = d3dDevice->CreateInputLayout(gpackingIputDesc, 3, vsBytecode->GetBufferPointer(), vsBytecode->GetBufferSize(), &m_modelInputLayout);

	if (FAILED(result))
	{
		MessageBox(NULL, "Error creating the model input layout", NULL, NULL);
		if (vsBytecode)
			vsBytecode->Release();
		return false;
	}

	vsBytecode->Release();

	compileResult = CompileD3DShader(L"DeferredLighting.fx", "VS_Main", "vs_5_0", &vsBytecode);
	if (!compileResult)
	{
		MessageBox(NULL, "Error compiling the deferred vertex shader!", NULL, NULL);
		return false;
	}
	result = d3dDevice->CreateVertexShader(vsBytecode->GetBufferPointer(), vsBytecode->GetBufferSize(), NULL, &m_deferredVShader);
	if (FAILED(result))
	{
		MessageBox(NULL, "Error creating the deferred vertex shader!", NULL, NULL);
		return false;
	}

	result = d3dDevice->CreateInputLayout(deferredShadingIputDesc, 2, vsBytecode->GetBufferPointer(), vsBytecode->GetBufferSize(), &m_squareInputLayout);
	if (FAILED(result))
	{
		MessageBox(NULL, "Error creating the deferred Input layout!", NULL, NULL);
		return false;
	}
	vsBytecode->Release();

	compileResult = CompileD3DShader(L"PackingGBuffer.fx", "PS_Main", "ps_5_0", &psBytecode);
	if (!compileResult)
	{
		MessageBox(NULL, "Error compiling gbuffer pixel shader!", NULL, NULL);
		return false;
	}
	result = d3dDevice->CreatePixelShader(psBytecode->GetBufferPointer(), psBytecode->GetBufferSize(), NULL, &m_gPackingPShader);
	if (FAILED(result))
	{
		MessageBox(NULL, "Error creating gbuffer packing pixel shader!", NULL, NULL);
		return false;
	}
	psBytecode->Release();

	compileResult = CompileD3DShader(L"DeferredLighting.fx", "PS_Main", "ps_5_0", &psBytecode);
	if (!compileResult)
	{
		MessageBox(NULL, "Error compiling deferred Lighting pixel shader!", NULL, NULL);
		return false;
	}
	result = d3dDevice->CreatePixelShader(psBytecode->GetBufferPointer(), psBytecode->GetBufferSize(), NULL, &m_deferredPShader);
	if (FAILED(result))
	{
		MessageBox(NULL, "Error creating deferred shading pixel shader!", NULL, NULL);
		return false;
	}
	psBytecode->Release();
	compileResult = CompileD3DShader(L"FinalSquarePS.hlsl", "main", "ps_5_0", &psBytecode);
	if (!compileResult)
	{
		MessageBox(NULL, "Error compiling final pixel shader!", NULL, NULL);
		return false;
	}
	result = d3dDevice->CreatePixelShader(psBytecode->GetBufferPointer(), psBytecode->GetBufferSize(), NULL, &m_finalSquarePS);
	if (FAILED(result))
	{
		MessageBox(NULL, "Error creating  final PS!", NULL, NULL);
		return false;
	};
	psBytecode->Release();
	vsBytecode = nullptr;
	psBytecode = nullptr;

	//Sampler state desc
	D3D11_SAMPLER_DESC pointSamplerDesc;
	ZeroMemory(&pointSamplerDesc, sizeof(pointSamplerDesc));
	pointSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	pointSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	pointSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	pointSamplerDesc.BorderColor[0] = 0.0f;
	pointSamplerDesc.BorderColor[1] = 0.0f;
	pointSamplerDesc.BorderColor[2] = 0.0f;
	pointSamplerDesc.BorderColor[3] = 0.0f;
	pointSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	pointSamplerDesc.MipLODBias = 0.0f;
	pointSamplerDesc.MaxAnisotropy = 1;
	pointSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	pointSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	
	result = d3dDevice->CreateSamplerState(&pointSamplerDesc, &m_PointSampler);

	//Light Properties buffer
	D3D11_BUFFER_DESC lightDesc;
	ZeroMemory(&lightDesc, sizeof(lightDesc));
	lightDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightDesc.ByteWidth = sizeof(LightSource);
	lightDesc.Usage = D3D11_USAGE_DEFAULT;

	result = d3dDevice->CreateBuffer(&lightDesc, NULL, &m_LightProperties);
	if (FAILED(result))
	{
		MessageBox(NULL, "Error creating light properties constant buffer", NULL, NULL);
		return false;
	}

	lightDesc.ByteWidth = sizeof(UniqueLights);
	result = d3dDevice->CreateBuffer(&lightDesc, NULL, &m_diffAmbCB);
	if (FAILED(result))
	{
		MessageBox(NULL, "Error creating default diffuse and abient constant buffer", NULL, NULL);
		return false;
	}
	diffAmb.AmbientDown = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	diffAmb.AmbientRange = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);

	diffAmb.DiffuseDirection = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	diffAmb.DiffuseIntensity = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);


	return true;
}

bool LightShader::InitGBuffers(ID3D11Device* d3dDevice)
{
	HRESULT result;
	int i;

	for (i = 0; i < G_BUFFER_COUNT; i++)
	{
		if (m_GBuffers[i]) m_GBuffers[i]->Release();
		if (m_ShaderResourceViews[i]) m_ShaderResourceViews[i]->Release();
		if (m_RenderTargetViews[i]) m_RenderTargetViews[i]->Release();
	}

	if (m_DepthTexture) m_DepthTexture->Release();
	if (m_DepthStencilView) m_DepthStencilView->Release();
	if (m_squareVertexBuffer) m_squareVertexBuffer->Release();

	D3D11_TEXTURE2D_DESC gTexDesc;
	ZeroMemory(&gTexDesc, sizeof(gTexDesc));
	
	gTexDesc.MipLevels = 1;
	gTexDesc.ArraySize = 1;
	gTexDesc.Width = textureWidth;
	gTexDesc.Height = textureHeight;
	gTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	gTexDesc.SampleDesc.Count = 1;
	gTexDesc.Usage = D3D11_USAGE_DEFAULT;
	gTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	gTexDesc.CPUAccessFlags = 0;
	gTexDesc.MiscFlags = 0;

	
	for (i = 0; i < G_BUFFER_COUNT; i++)
	{
		result = d3dDevice->CreateTexture2D(&gTexDesc, NULL, &m_GBuffers[i]);
		if (FAILED(result))
		{
			MessageBox(NULL, "Cannot create gbuffer texture 2D", NULL, 0);
			return false;
		}
	}

	D3D11_RENDER_TARGET_VIEW_DESC gTargetDesc;
	ZeroMemory(&gTargetDesc, sizeof(gTargetDesc));
	gTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	gTargetDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	gTargetDesc.Texture2D.MipSlice = 0;
	
	for (i = 0; i < G_BUFFER_COUNT; i++)
	{
	
		result = d3dDevice->CreateRenderTargetView(m_GBuffers[i], &gTargetDesc, &m_RenderTargetViews[i]);
		if (FAILED(result))
		{
			MessageBox(NULL, "Cannot create gbuffer Render target view", NULL, 0);
			return false;
		}
	}
	// Setup the description of the shader resource view.
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource views.
	for (i = 0; i < G_BUFFER_COUNT; i++)
	{
		 
		result = d3dDevice->CreateShaderResourceView(m_GBuffers[i], &shaderResourceViewDesc, &m_ShaderResourceViews[i]);
		if (FAILED(result))
		{
			
			MessageBox(NULL, "Cannot create gbuffer Shader resource view", NULL, 0);
			return false;
		}
	}
	// Create the Depth stencil texture
	D3D11_TEXTURE2D_DESC depthTexDesc;
	ZeroMemory(&depthTexDesc, sizeof(depthTexDesc));
	depthTexDesc.Width = textureWidth;
	depthTexDesc.Height = textureHeight;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;

	result = d3dDevice->CreateTexture2D(&depthTexDesc, NULL, &m_DepthTexture);

		if (FAILED(result))
		{
			MessageBox(0, "Failed to create the depth texture!",0 ,0);
			return false;
		}

		// Create the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = depthTexDesc.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;

		result = d3dDevice->CreateDepthStencilView(m_DepthTexture, &descDSV, &m_DepthStencilView);

		if (FAILED(result))
		{
			MessageBox(0, "Failed to create the depth stencil target view!", 0 , 0);
			return false;
		}

		float right = static_cast<float>(textureWidth) / 2.0f;
		float left = -right;
		float top	= static_cast<float>(textureHeight) / 2.0f;
		float bottom = -top;

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

		D3D11_BUFFER_DESC squareDesc;
		ZeroMemory(&squareDesc, sizeof(squareDesc));
		squareDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		squareDesc.ByteWidth = sizeof(windowSquare);
		squareDesc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA squareSubResource;
		ZeroMemory(&squareSubResource, sizeof(squareSubResource));
		squareSubResource.pSysMem = windowSquare;

		result = d3dDevice->CreateBuffer(&squareDesc, &squareSubResource, &m_squareVertexBuffer);
		if (FAILED(result))
		{
			MessageBox(0, "Failed to create square vertex buffer", 0, 0);
			return false;
		}
		
		
		return true;
}

void LightShader::PreparePacking(ID3D11DeviceContext* context)
{

	context->IASetInputLayout(m_modelInputLayout);
	context->VSSetShader(m_gPackingVShader, 0, 0);
	context->PSSetShader(m_gPackingPShader, 0, 0);
	context->OMSetRenderTargets(G_BUFFER_COUNT - 1, m_RenderTargetViews, m_DepthStencilView);
	for (UINT i = 0; i < G_BUFFER_COUNT - 1; i++)
	{
		context->ClearRenderTargetView(m_RenderTargetViews[i], clearColor);
	}
	context->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->UpdateSubresource(m_diffAmbCB, 0, 0, &diffAmb, 0, 0);
	context->PSSetConstantBuffers(0, 1, &m_diffAmbCB);
}

void LightShader::RenderDeferred(ID3D11DeviceContext* context)
{

	context->IASetInputLayout(m_squareInputLayout);
	context->IASetVertexBuffers(0, 1, &m_squareVertexBuffer, &stride, &offset);
	context->VSSetShader(m_deferredVShader, 0, 0);
	context->PSSetShader(m_deferredPShader, 0, 0);
	context->PSSetSamplers(0, 1, &m_PointSampler);

	int j = -1, k = G_BUFFER_COUNT - 2;
	context->PSSetShaderResources(0, 4, m_ShaderResourceViews);
	for (UINT i = 0; i < m_totalLights; i++)
	{
		j = j*(-1);
		k = k + j;
		context->OMSetRenderTargets(1, &m_RenderTargetViews[k], 0);
		context->ClearRenderTargetView(m_RenderTargetViews[k], clearColor);
		context->PSSetShaderResources(4, 1, &m_ShaderResourceViews[k + (j*-1)]);
		context->UpdateSubresource(m_LightProperties, 0, 0, &m_lightSources[i], 0, 0);
		context->PSSetConstantBuffers(0, 1, &m_LightProperties);
		context->Draw(6, 0);
	}
	context->OMSetRenderTargets(1, &m_RenderTargetViews[2], 0);
	context->PSSetShader(m_finalSquarePS, 0, 0);
	
	context->PSSetShaderResources(0, 1, &m_ShaderResourceViews[0]);
	if(k == 5)
		context->PSSetShaderResources(1, 1, &m_ShaderResourceViews[5]);
	else
		context->PSSetShaderResources(1, 1, &m_ShaderResourceViews[4]);
}

void LightShader::Release()
{
	for (int i = 0; i < G_BUFFER_COUNT; i++)
	{
		m_GBuffers[i]->Release();
		m_ShaderResourceViews[i]->Release();
		m_RenderTargetViews[i]->Release();
		
		m_GBuffers[i] = 0;
		m_ShaderResourceViews[i] = 0;
		m_RenderTargetViews[i] = 0;
	}

	m_DepthTexture->Release();
	m_DepthStencilView->Release();
	m_squareVertexBuffer->Release();
	m_modelInputLayout->Release();
	m_squareInputLayout->Release();
	m_gPackingVShader->Release();
	m_gPackingPShader->Release();
	m_deferredVShader->Release();
	m_deferredPShader->Release();
	m_finalSquarePS->Release();
	m_LightProperties->Release();
	m_PointSampler->Release();


	m_DepthTexture = 0;
	m_DepthStencilView = 0;
	m_squareVertexBuffer = 0;
	m_modelInputLayout = 0;
	m_squareInputLayout = 0;
	m_gPackingVShader = 0;
	m_gPackingPShader = 0;
	m_deferredVShader = 0;
	m_deferredPShader = 0;
	m_finalSquarePS = 0;
	m_LightProperties = 0;
	m_PointSampler = 0;

}

LightShader::~LightShader()
{
	Release();
}