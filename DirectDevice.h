#ifndef DIRECT_DEVICE_H
#define DIRECT_DEVICE_H

#include <d3d11.h>
#include <D3DCompiler.h>
#include <DirectXMath.h>


using namespace DirectX;

class DirectDevice
{
public:
	DirectDevice();
	bool Initialize(HWND, bool);
	void Relase();
	void swapChainSetNewSize(HWND hwnd);
	bool createViewsDependWindowSize();
	void Clear();
	void Present();
	void  Message(LPCSTR);
	bool CompileD3DShader(LPCWSTR filePath, LPCSTR entry, LPCSTR shaderModel, ID3DBlob** buffer);
	~DirectDevice();
	void getWindowSize(HWND hwnd);
	ID3D11Device*			d3dDevice_;
	ID3D11DeviceContext*	d3dContext_;

	
	unsigned int			windowWidth;
	unsigned int			windowHeight;

private:
	
	struct HemiColor 
	{
		XMFLOAT4	AmbientDown = { 0.2f, 0.2f, 0.2f, 0.0f };
		XMFLOAT4	AmbientRange = { 0.0f, 0.3f, 0.3f, 0.0f };
	};
	
	HemiColor				hemiColor_;
	ID3D11RenderTargetView*	renderTargetView_;
	IDXGISwapChain*			swapChain_;

	D3D_DRIVER_TYPE			driverType_;
	D3D_FEATURE_LEVEL		featureLevel_;
	
	XMMATRIX				projMatrix_;
	ID3D11Buffer*			projCB_; //Project constant buffer
	ID3D11Buffer*			constHemispheric_;

	//Depth Stenci

	ID3D11Texture2D* depthTexture_;
	ID3D11DepthStencilView* depthStencilView_;


};

#endif