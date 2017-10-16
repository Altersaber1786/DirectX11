#ifndef DIRECT_DEVICE_H
#define DIRECT_DEVICE_H

#include <D3DCompiler.h>
#include <DirectXMath.h>
#include "d3dx11effect.h"

using namespace DirectX;

class DirectDevice
{
public:
	DirectDevice();
	bool Initialize(HWND, bool);
	void Relase();

	void  Message(LPCSTR);
	~DirectDevice();
	void getWindowSize();
	ID3D11Device*			d3dDevice_;
	ID3D11DeviceContext*	d3dContext_;

	
	UINT			windowWidth;
	UINT			windowHeight;

	IDXGISwapChain*			swapChain_;
private:
	
	D3D_DRIVER_TYPE			driverType_;
	D3D_FEATURE_LEVEL		featureLevel_;
	
	HWND					handleWindow;
	ID3D11Buffer*			constHemispheric_;


};

#endif