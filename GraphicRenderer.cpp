#include "GraphicRenderer.h"


GraphicRenderer::GraphicRenderer()
{

};
bool GraphicRenderer::LoadGeometry(char* meshFile, GameModel* model)
{
	//Load Vertex buffer
	MeshLoader* m_Loader = new MeshLoader();
	if (!(m_Loader->LoadMeshFromOBJ(meshFile, m_device, model)))
	{
		MessageBox(NULL, "Error creating vertex buffer", NULL, NULL);
		return false;
	}
	delete m_Loader;

	return true;
}
bool GraphicRenderer::LoadModelList()
{
	//GameModel* wall = new GameModel();
	//if (!LoadGeometry("./ModelsandTextures/wall.obj", wall))
		//return false;
	GameModel* business = new GameModel();
	if (!LoadGeometry("./ModelsandTextures/house_obj.obj", business))
		return false;

	//OpaqueModelList.push_back(wall);
	OpaqueModelList.push_back(business);
	totalOpaqueModels = static_cast<int>(OpaqueModelList.size());
	totalTransparentModels = static_cast<int>(TransparentModelList.size());
	return true;
}

bool GraphicRenderer::LoadObject(GameObject* Object)
{
	Object->InitWorldCB(m_device->d3dDevice_);
	for (UINT i = 0; i < totalOpaqueModels; i++)
	{
		if (Object->modelIndex == i)
		{			
			OpaqueModelList[i]->pushObject(Object);
			return true;
		};
	}
	MessageBox(NULL, "Can't find object's 3D model", NULL, 0);
	return false;
}

bool GraphicRenderer::LoadCamera(Camera* cam)
{
	if (!cam->Initialize(m_device->d3dDevice_))
		return false;
	camera = cam;
	return true;
}

bool GraphicRenderer::Initialize(HWND hwnd)
{
	m_device = new DirectDevice();
	if (!m_device->Initialize(hwnd, true))
	{
		return false;
	};
	windowWidth = m_device->windowWidth;
	windowHeight = m_device->windowHeight;
	bool result;
	m_deferredLighting = new LightShader();
	m_deferredLighting->textureWidth = windowWidth;
	m_deferredLighting->textureHeight = windowHeight;
	result = m_deferredLighting->InitGBuffers(m_device->d3dDevice_);
	if (result == false)
		return false;

	result = m_deferredLighting->Initialize(m_device);
	if (result == false)
		return false;

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

	if (FAILED(m_device->d3dDevice_->CreateSamplerState(&samplerDesc, &samplerState_)))
	{
		MessageBox(NULL, "Failed to create sampler state", NULL, NULL);
		return false;
	}


	
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	m_device->d3dDevice_->CreateDepthStencilState(&depthStencilDesc, &m_DepthEnableState);


	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	m_device->d3dDevice_->CreateDepthStencilState(&depthDisabledStencilDesc, &m_DepthDisableState);

	D3D11_BLEND_DESC transparentDesc;
	ZeroMemory(&transparentDesc, sizeof(transparentDesc));
	transparentDesc.AlphaToCoverageEnable = false;
	transparentDesc.IndependentBlendEnable = false;

	transparentDesc.RenderTarget[0].BlendEnable = false;
	transparentDesc.RenderTarget[1].BlendEnable = false;
	transparentDesc.RenderTarget[2].BlendEnable = false;
	transparentDesc.RenderTarget[3].BlendEnable = false;
	transparentDesc.RenderTarget[4].BlendEnable = false;


	transparentDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
	transparentDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
	transparentDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparentDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	if (FAILED(m_device->d3dDevice_->CreateBlendState(&transparentDesc, &m_TransparentBS)))
		return false;
	transparentDesc.IndependentBlendEnable = false;
	transparentDesc.RenderTarget[0].BlendEnable = false;
	if (FAILED(m_device->d3dDevice_->CreateBlendState(&transparentDesc, &m_offBS)))
		return false;
	
	if (!CreateViewsDependWindowSize())
	{
		return false;
	}

	m_device->d3dContext_->OMSetDepthStencilState(m_DepthEnableState, 1);

	return true;
}

bool GraphicRenderer::CreateViewsDependWindowSize()
{
	m_deferredLighting->textureWidth = windowWidth;
	m_deferredLighting->textureHeight = windowHeight;
	ID3D11Texture2D* BackBuffer;
	if (FAILED(m_device->swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer)))
	{
		MessageBox(NULL,"Failed to get the swap chain back buffer!", NULL, NULL);
		return false;
	};

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	HRESULT result = m_device->d3dDevice_->CreateRenderTargetView(BackBuffer, &rtvDesc, &m_backBufferTarget);
	if (BackBuffer)
		BackBuffer->Release();
	if (FAILED(result))
	{
		MessageBox(NULL, "Failed to create the render target view!", NULL, 0);
		return false;
	};

	float AspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
	projMatrix_ = XMMatrixPerspectiveFovLH(XM_PI / 4.0f, AspectRatio, 0.1f, 600.0f);
	
	viewport.Width = static_cast<float>(windowWidth);
	viewport.Height = static_cast<float>(windowHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = D3D11_CULL_NONE;
	m_device->d3dDevice_->CreateRasterizerState(&rsDesc, &m_RSWireFrame);
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	m_device->d3dDevice_->CreateRasterizerState(&rsDesc, &m_RSNormal);

	
	// Create the Depth stencil texture
	D3D11_TEXTURE2D_DESC depthTexDesc;
	ZeroMemory(&depthTexDesc, sizeof(depthTexDesc));
	depthTexDesc.Width = windowWidth;
	depthTexDesc.Height = windowHeight;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;

	result = m_device->d3dDevice_->CreateTexture2D(&depthTexDesc, NULL, &m_DepthTexture);

	if (FAILED(result))
	{
		MessageBox(0, "Failed to create the depth texture!", 0, 0);
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = depthTexDesc.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	result = m_device->d3dDevice_->CreateDepthStencilView(m_DepthTexture, &descDSV, &m_DepthStencilView);
	return true;
}

void GraphicRenderer::SetBuffersNewSize()
{
	m_device->getWindowSize();
	windowWidth = m_device->windowWidth;
	windowHeight = m_device->windowHeight;
	if (windowWidth > 0 && windowHeight > 0)
	{
		m_backBufferTarget->Release();
		if (FAILED(m_device->swapChain_->ResizeBuffers(1, windowWidth, windowHeight, DXGI_FORMAT_UNKNOWN, NULL)))
		{
			MessageBox(NULL, "Failed to resize the buffer", NULL, NULL);
		};
		CreateViewsDependWindowSize();
		m_deferredLighting->InitGBuffers(m_device->d3dDevice_);
	}
}

void GraphicRenderer::Update()
{
	for (UINT i = 0; i < totalOpaqueModels; i++)
	{
		OpaqueModelList[i]->Update();
	}
	for (UINT i = 0; i < totalTransparentModels; i++)
	{
		TransparentModelList[i]->Update();
	}
	camera->Update(m_device);
}

void GraphicRenderer::BeginScene()
{
	m_device->d3dContext_->RSSetViewports(1, &viewport);
	m_device->d3dContext_->ClearRenderTargetView(m_backBufferTarget, clearColor);
	m_device->d3dContext_->PSSetSamplers(0, 1, &samplerState_);
	m_deferredLighting->PreparePacking(m_device->d3dContext_);

}

void GraphicRenderer::EndScene()
{
	m_device->swapChain_->Present(0, 0);

}
void GraphicRenderer::SetNormalState()
{
	m_device->d3dContext_->RSSetState(m_RSNormal);
}

void GraphicRenderer::SetWireFrameState()
{
	m_device->d3dContext_->RSSetState(m_RSWireFrame);
}
void GraphicRenderer::Render()
{
	BeginScene();
	//m_device->d3dContext_->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	//m_device->d3dContext_->OMSetRenderTargets(1, &m_backBufferTarget, m_DepthStencilView);
	//m_device->d3dContext_->OMSetBlendState(m_TransparentBS, blendFactors, 0xffffffff);
	for (UINT i = 0; i < totalOpaqueModels; i++)
	{
		OpaqueModelList[i]->Render(m_device, camera->viewMatrix, projMatrix_);
	}
	m_device->d3dContext_->OMSetBlendState(NULL, NULL, 0xffffffff);
	m_device->d3dContext_->RSSetState(m_RSNormal);
	m_device->d3dContext_->OMSetRenderTargets(0, NULL, 0);
	m_device->d3dContext_->PSSetConstantBuffers(1, 1, camera->GetPosCB());
	m_deferredLighting->RenderDeferred(m_device->d3dContext_);
	m_device->d3dContext_->OMSetRenderTargets(1, &m_backBufferTarget, NULL);
	m_device->d3dContext_->Draw(6, 0);
	EndScene();
};

void GraphicRenderer::Release()
{

	for (UINT i = 0; i < totalOpaqueModels; i++)
	{
		delete OpaqueModelList[i];
	}
	OpaqueModelList.clear();
	for (UINT i = 0; i < totalTransparentModels; i++)
	{
		delete TransparentModelList[i];
	}
	TransparentModelList.clear();
	delete m_device;
	delete m_deferredLighting;
	m_deferredLighting = 0;
	if(m_backBufferTarget!= nullptr)m_backBufferTarget->Release();
	m_backBufferTarget = 0;
	if (m_DepthEnableState != nullptr)m_DepthEnableState->Release();
	m_DepthEnableState = 0;
	if (m_DepthDisableState != nullptr)m_DepthDisableState->Release();
	m_DepthDisableState = 0;
	if (m_RSWireFrame != nullptr)m_RSWireFrame->Release();
	m_RSWireFrame = 0;
	if (m_RSNormal != nullptr)m_RSNormal->Release();
	m_RSNormal = 0;
	if (m_TransparentBS != nullptr)m_TransparentBS->Release();
	m_TransparentBS = 0;
	if (m_offBS != nullptr)m_offBS->Release();
	m_offBS = 0;
	if(m_DepthStencilView != nullptr)m_DepthStencilView->Release();
	m_DepthStencilView = 0;
	if(m_DepthTexture != nullptr)m_DepthTexture->Release();
	m_DepthTexture = 0;
	camera = 0;
}


GraphicRenderer::~GraphicRenderer()
{
	this->Release();
};