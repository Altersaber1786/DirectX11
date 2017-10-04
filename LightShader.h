#ifndef LIGHT_SHADER_H
#define LIGHT_SHADER_H

#include "DirectDevice.h"
#include <vector>
#include "GameMaterial.h"

#define G_BUFFER_COUNT 6

using namespace DirectX;

class LightShader
{
public:
	ID3D11DepthStencilView*		m_DepthStencilView;
	LightShader();
	~LightShader();
	
	bool InitGBuffers(ID3D11Device* device);
	bool Initialize(ID3D11Device* device);
	void PreparePacking(ID3D11DeviceContext* context);
	void RenderDeferred(ID3D11DeviceContext* context);
	void Release();

public:
	UINT textureWidth;
	UINT textureHeight;

private:
	bool CompileD3DShader(LPCWSTR filePath, LPCSTR entry, LPCSTR shaderModel, ID3DBlob** buffer);

	struct UniqueLights
	{
		XMFLOAT4	AmbientDown;
		XMFLOAT4	AmbientRange;
		XMFLOAT4	DiffuseIntensity;
		XMFLOAT4	DiffuseDirection;
	} diffAmb;
	
	struct LightSource
	{
		XMFLOAT4 position;
		XMFLOAT4 intensity;
	};

	struct Vertex2D
	{
		XMFLOAT3 position;
		XMFLOAT2 texcoord;
	};

private:

	ID3D11Texture2D*			m_GBuffers[G_BUFFER_COUNT];
	ID3D11ShaderResourceView*	m_ShaderResourceViews[G_BUFFER_COUNT];
	ID3D11RenderTargetView*		m_RenderTargetViews[G_BUFFER_COUNT];
	ID3D11InputLayout*			m_modelInputLayout;
	ID3D11InputLayout*			m_squareInputLayout;
	ID3D11SamplerState*			m_PointSampler;


	ID3D11Texture2D*			m_DepthTexture;


	ID3D11VertexShader*			m_gPackingVShader;
	ID3D11PixelShader*			m_gPackingPShader;
	ID3D11VertexShader*			m_deferredVShader;
	ID3D11PixelShader*			m_deferredPShader;
	ID3D11PixelShader*			m_finalSquarePS;

	ID3D11Buffer*				m_LightProperties;
	ID3D11Buffer*				m_squareVertexBuffer;
	ID3D11Buffer*				m_diffAmbCB;

	std::vector<LightSource>	m_lightSources;
	UINT						m_totalLights;
	UINT						stride = sizeof(Vertex2D);
	UINT						offset = 0;
	Vertex2D					windowSquare[6];
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
};

#endif