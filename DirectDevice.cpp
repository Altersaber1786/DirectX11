#include "DirectDevice.h"

using namespace DirectX;

DirectDevice::DirectDevice()
{
	featureLevel_ = D3D_FEATURE_LEVEL_11_0;
	d3dDevice_ = NULL;
	d3dContext_ = NULL;
	swapChain_ = NULL;
	windowWidth = 0;
	windowHeight = 0;
}

bool DirectDevice::Initialize(HWND hwnd, bool windowed)
{
	//Describe and Create Swapchain & Direct3D device
	
	//Get Window size for Swapchain.Buffer description
	handleWindow = hwnd;
	getWindowSize();

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
	return true;
}

void DirectDevice::getWindowSize()
{
	RECT dimensions;
	GetClientRect(handleWindow, &dimensions);
	windowWidth = dimensions.right - dimensions.left;
	windowHeight = dimensions.bottom - dimensions.top;
};


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

	d3dContext_->Release();
	d3dDevice_->Release();
	swapChain_->Release();
	d3dContext_ = 0;
	d3dDevice_ = 0;
	swapChain_ = 0;
};