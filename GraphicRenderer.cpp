#include "GraphicRenderer.h"


GraphicRenderer::GraphicRenderer()
{

};

bool GraphicRenderer::LoadModelList()
{
	GameMaterial* mat01 = new GameMaterial();
	if (!mat01->LoadContent(m_device, L"Rengeg.DDS"))
		return false;

	GameModel* sphere = new GameModel();
	if (!sphere->LoadGeometry(m_device, "Sphere.obj"))
		return false;
	ModelList.push_back(sphere);
	totalModels = static_cast<int>(ModelList.size());

	MaterialList.push_back(mat01);
	totalMaterials = static_cast<int>(MaterialList.size());
	return true;
}

bool GraphicRenderer::LoadObject(GameObject* Object)
{
	Object->InitWorldCB(m_device->d3dDevice_);
	for (UINT i = 0; i < totalModels; i++)
	{
		if (Object->modelIndex == i)
		{			
			Object->Set3DModel(ModelList[i]);
			MaterialList[0]->pushObject(Object);
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

	result = m_deferredLighting->Initialize(m_device->d3dDevice_);
	if (result == false)
		return false;

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


	if (!CreateViewsDependWindowSize())
	{
		return false;
	}

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
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
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
	projMatrix_ = XMMatrixPerspectiveFovLH(XM_PI / 4.0f, AspectRatio, 0.1f, 100.0f);
	
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

	m_device->d3dContext_->RSSetViewports(1, &viewport);
	return true;
}

void GraphicRenderer::SetBuffersNewSize()
{
	m_device->getWindowSize();
	windowWidth = m_device->windowWidth;
	windowHeight = m_device->windowHeight;
	m_backBufferTarget->Release();
	if (FAILED(m_device->swapChain_->ResizeBuffers(1, windowWidth, windowHeight, DXGI_FORMAT_UNKNOWN, NULL)))
	{
		MessageBox(NULL, "Failed to resize the buffer", NULL, NULL);
	};
	CreateViewsDependWindowSize();
	m_deferredLighting->InitGBuffers(m_device->d3dDevice_);

}

void GraphicRenderer::Update()
{
	for (UINT i = 0; i < totalMaterials; i++)
	{
		MaterialList[i]->Update();
	}
	camera->Update(m_device);
}

void GraphicRenderer::BeginScene()
{
	m_device->d3dContext_->ClearRenderTargetView(m_backBufferTarget, clearColor);
	m_deferredLighting->PreparePacking(m_device->d3dContext_);
}

void GraphicRenderer::EndScene()
{
	m_device->swapChain_->Present(0, 0);

}

void GraphicRenderer::Render()
{
	BeginScene();
	m_device->d3dContext_->RSSetState(m_RSWireFrame);
	m_device->d3dContext_->OMSetDepthStencilState(m_DepthEnableState, 1);

	for (UINT i = 0; i < totalMaterials; i++)
	{
		MaterialList[i]->Render(m_device, camera->viewMatrix, projMatrix_);
	}
	m_device->d3dContext_->RSSetState(m_RSNormal);
	m_device->d3dContext_->OMSetRenderTargets(1, &m_backBufferTarget, 0);
	m_device->d3dContext_->OMSetDepthStencilState(m_DepthDisableState, 1);
	m_device->d3dContext_->PSSetConstantBuffers(1, 1, camera->GetPosCB());
	m_deferredLighting->RenderDeferred(m_device->d3dContext_);
	m_device->d3dContext_->OMSetRenderTargets(1, &m_backBufferTarget, 0);
	m_device->d3dContext_->Draw(6, 0);
	EndScene();
};

void GraphicRenderer::Release()
{
	for (UINT i = 0; i < totalMaterials; i++)
	{
		for (UINT j = 0; j < MaterialList[i]->totalObjects; j++)
		{
			delete MaterialList[i]->ObjectList[j];
		}
		MaterialList[i]->ObjectList.clear();
		delete MaterialList[i];
	}
	MaterialList.clear();
	for (UINT i = 0; i < totalModels; i++)
	{
		delete ModelList[i];
	}
	ModelList.clear();
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
	camera = 0;
}


GraphicRenderer::~GraphicRenderer()
{
	Release();
};