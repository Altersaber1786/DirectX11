#ifndef DIRECT_DEVICE_H
#define DIRECT_DEVICE_H

#include <D3DCompiler.h>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class DirectDevice
{
public:
	DirectDevice();
	bool Initialize(HWND, bool);
	bool CompileD3DShader(LPCWSTR filePath, LPCSTR entry, LPCSTR shaderModel, ID3DBlob** buffer);
	void Relase();

	void  Message(LPCSTR);
	~DirectDevice();
	void getWindowSize();
	ID3D11Device*			d3dDevice_;
	ID3D11DeviceContext*	d3dContext_;

	
	UINT			windowWidth;
	UINT			windowHeight;

	IDXGISwapChain*			swapChain_;

	struct Vertex2D
	{
		XMFLOAT3	position;
		XMFLOAT2	texcoord;
	};
private:
	
	D3D_DRIVER_TYPE			driverType_;
	D3D_FEATURE_LEVEL		featureLevel_;
	
	HWND					handleWindow;
	ID3D11Buffer*			constHemispheric_;


};

#endif