#ifndef LIGHT_SHADER_H
#define LIGHT_SHADER_H

#include "DirectDevice.h"
#include <vector>
#include "GameMaterial.h"

#define G_BUFFER_COUNT	6
#define	TOTAL_LIGHT_TYPES	4;

using namespace DirectX;

enum LIGHT_TYPE
{
	LIGHT_TYPE_DIRECTIONAL	=	0,
	LIGHT_TYPE_POINT		=	1,
	LIGHT_TYPE_SPOT			=	2,
	LIGHT_TYPE_CAPSULE		=	3
};

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
	
	struct DirectionalLight
	{
		XMFLOAT4	direction;
		XMFLOAT4	intensity;
	};
	struct PointLight
	{
		XMFLOAT4	position;
		XMFLOAT4	intensity;
	};
	struct SpotLight
	{
		XMFLOAT3	position;
		float		rangeRCP;
		XMFLOAT3	direction;
		float		cosOuterCone;
		XMFLOAT3	intensity;
		float		innerConeRCP;
	};
	struct CapsuleLight
	{
		XMFLOAT3	position;
		float		rangeRCP;
		XMFLOAT3	direction;
		float		len;
		XMFLOAT4	intensity;
	};

	struct LightSource
	
	{
		std::vector<DirectionalLight>	dirLights;
		std::vector<PointLight>			pointLights;
		std::vector<SpotLight>			spotLights;
		std::vector<CapsuleLight>		capsuleLights;
	};


	struct Vertex2D
	{
		XMFLOAT3	position;
		XMFLOAT2	texcoord;
	};
	
private:
	//Gbuffer views
	ID3D11Texture2D*			m_GBuffers[G_BUFFER_COUNT];
	ID3D11ShaderResourceView*	m_ShaderResourceViews[G_BUFFER_COUNT];
	ID3D11RenderTargetView*		m_RenderTargetViews[G_BUFFER_COUNT];

	//Unordered access view
	//ID3D11UnorderedAccessView*	m_UAV;

	ID3D11InputLayout*			m_modelInputLayout;
	ID3D11InputLayout*			m_squareInputLayout;
	ID3D11SamplerState*			m_PointSampler;


	ID3D11Texture2D*			m_DepthTexture;


	ID3D11VertexShader*			m_gPackingVShader;
	ID3D11PixelShader*			m_gPackingPShader;
	ID3D11VertexShader*			m_deferredVShader;
	ID3D11PixelShader*			m_deferredPShader;
	ID3D11PixelShader*			m_finalSquarePS;
	ID3D11HullShader*			m_hullShader;
	ID3D11DomainShader*			m_domainShader;

	ID3D11Buffer*				m_LightProperties;
	ID3D11Buffer*				m_squareVertexBuffer;
	ID3D11Buffer*				m_diffAmbCB;

	
	UINT						stride = sizeof(Vertex2D);
	UINT						offset = 0;
	Vertex2D					windowSquare[6];
	LightSource					m_LightSources;

	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
};

#endif