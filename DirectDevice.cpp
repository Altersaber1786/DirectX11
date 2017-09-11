#include "DirectDevice.h"

using namespace DirectX;

DirectDevice::DirectDevice()
{
	featureLevel_ = D3D_FEATURE_LEVEL_11_0;
	d3dDevice_ = NULL;
	d3dContext_ = NULL;
	swapChain_ = NULL;
	renderTargetView_ = NULL;
	windowWidth = 0;
	windowHeight = 0;
}

bool DirectDevice::Initialize(HWND hwnd, bool windowed)
{
	//Describe and Create Swapchain & Direct3D device
	
	//Get Window size for Swapchain.Buffer description
	getWindowSize(hwnd);


	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	SecureZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = windowWidth;
	swapChainDesc.BufferDesc.Height = windowHeight;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	 
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = windowed;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SampleDesc.Count = 1;

	D3D_FEATURE_LEVEL featureLevel[]
	{ 
	 D3D_FEATURE_LEVEL_11_0,
	 D3D_FEATURE_LEVEL_10_1, 
	 D3D_FEATURE_LEVEL_10_0,
	};	
unsigned int creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined (_DEBUG) 
	//creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	if (FAILED(D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE, 
		NULL,
		creationFlags, 
		featureLevel,
		ARRAYSIZE(featureLevel), 
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&swapChain_,
		&d3dDevice_,
		&featureLevel_,
		&d3dContext_
		)))
	{
		Message("Failed to create the Direct3D device!");
		return false;
	}

	createViewsDependWindowSize();
	//Constant project buffer
	D3D11_BUFFER_DESC constDesc;
	ZeroMemory(&constDesc, sizeof(constDesc));
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = sizeof(XMMATRIX);
	constDesc.Usage = D3D11_USAGE_DEFAULT;

	if (FAILED(d3dDevice_->CreateBuffer(&constDesc, 0, &projCB_)))
	{
		return false;
	}

	float AspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
	projMatrix_ = XMMatrixPerspectiveFovLH(XM_PI / 4.0f, AspectRatio, 0.1f, 100.0f);
	projMatrix_ = XMMatrixTranspose(projMatrix_);
	d3dContext_->UpdateSubresource(projCB_, 0, 0, &projMatrix_, 0, 0);
	d3dContext_->VSSetConstantBuffers(2, 1, &projCB_);

	return true;
}
bool DirectDevice::createViewsDependWindowSize()

{// Get the swapchain's first buffer

	ID3D11Texture2D* BackBuffer;
	if (FAILED(swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer)))
	{
		Message("Failed to get the swap chain back buffer!");
		return false;
	};

	// Create Render Target View
	// Render target is now represented by swapchain's first buffer
	HRESULT result = d3dDevice_->CreateRenderTargetView(BackBuffer, NULL, &renderTargetView_);
	if (BackBuffer)
		BackBuffer->Release();
	if (FAILED(result))
	{
		Message("Failed to create the render target view!");
		return false;
	};


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

	result = d3dDevice_->CreateTexture2D(&depthTexDesc, NULL, &depthTexture_);

	if (FAILED(result))
	{
		Message("Failed to create the depth texture!");
		return false;
	}

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = depthTexDesc.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	result = d3dDevice_->CreateDepthStencilView(depthTexture_, &descDSV, &depthStencilView_);

	if (FAILED(result))
	{
		Message("Failed to create the depth stencil target view!");
		return false;
	}

	//Output merge set render target
	d3dContext_->OMSetRenderTargets(1, &renderTargetView_, depthStencilView_);

	//Rasterizer Stage set view port
	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(windowWidth);
	viewport.Height = static_cast<float>(windowHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	d3dContext_->RSSetViewports(1, &viewport);

}
bool DirectDevice::CompileD3DShader(LPCWSTR filePath, LPCSTR entry, LPCSTR shaderModel, ID3DBlob** buffer)
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

	result = D3DCompileFromFile(filePath, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry, shaderModel,shaderFlags, 0, buffer, &errorBuffer);

	if (FAILED(result))
	{
		Message("Error compile shader code form file!");
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

void DirectDevice::getWindowSize(HWND hwnd)
{
	RECT dimensions;
	GetClientRect(hwnd, &dimensions);
	windowWidth = dimensions.right - dimensions.left;
	windowHeight = dimensions.bottom - dimensions.top;
};
void DirectDevice::swapChainSetNewSize(HWND hwnd)
{
	getWindowSize(hwnd);
	if (depthTexture_)		depthTexture_->Release();
	if (depthStencilView_)	depthStencilView_->Release();
	if (renderTargetView_)	renderTargetView_->Release();
	if (FAILED(swapChain_->ResizeBuffers(1, windowWidth, windowHeight, DXGI_FORMAT_UNKNOWN, NULL)))
	{
		Message("Failed to resize the buffer");
	};

	createViewsDependWindowSize();

	float AspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
	projMatrix_ = XMMatrixPerspectiveFovLH(XM_PI / 4.0f, AspectRatio, 0.1f, 100.0f);
	projMatrix_ = XMMatrixTranspose(projMatrix_);
	d3dContext_->UpdateSubresource(projCB_, 0, 0, &projMatrix_, 0, 0);
	d3dContext_->VSSetConstantBuffers(2, 1, &projCB_);

};

void DirectDevice::Clear()
{

	if (d3dContext_ == 0)
		return;
	float clearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	d3dContext_->ClearRenderTargetView(renderTargetView_, clearColor);
	d3dContext_->ClearDepthStencilView(depthStencilView_, D3D11_CLEAR_DEPTH, 1.0f, 0);
	
}

void DirectDevice::Present()
{
	swapChain_->Present(0, 0);
}

DirectDevice::~DirectDevice()
{
	this->Relase();
}

void DirectDevice::Message(LPCSTR x) 
{
	MessageBox(NULL, x, "Error", NULL);
}

void DirectDevice::Relase()
{
	if (depthTexture_)		depthTexture_->Release();
	if (depthStencilView_)	depthStencilView_->Release();
	if (d3dContext_)		d3dContext_->Release();
	if (d3dDevice_)			d3dDevice_->Release();
	if (swapChain_)			swapChain_->Release();
	if (renderTargetView_)	renderTargetView_->Release();
	if (projCB_)			projCB_->Release();
	return;
};