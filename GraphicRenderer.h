#ifndef GRAPHIC_RENDERER_H
#define GRAPHIC_RENDERER_H

#include "LightShader.h"
#include "GameModel.h"
#include "Camera.h"
#include "MeshLoader.h"
class GraphicRenderer
{

public:
	D3D11_VIEWPORT viewport;
	GraphicRenderer();
	~GraphicRenderer();
	bool Initialize(HWND hwnd);
	bool LoadModelList();
	void SetBuffersNewSize();
	void Render();
	void Release();
	void Update();
	bool CreateViewsDependWindowSize();
	bool LoadCamera(Camera* cam);
	bool LoadObject(GameObject* Object);
	void SetNormalState();
	void SetWireFrameState();
	XMMATRIX	projMatrix_;
private:
	bool LoadGeometry(char* meshFile, GameModel* model);
	void BeginScene();
	void EndScene();
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float blendFactors[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	Camera*		camera;

	UINT		totalOpaqueModels;
	UINT		totalTransparentModels;
	UINT		windowWidth;
	UINT		windowHeight;

	std::vector<GameModel*>		OpaqueModelList;
	std::vector<GameModel*>		TransparentModelList;

	LightShader*				m_deferredLighting;
	ID3D11RenderTargetView*		m_backBufferTarget;

	ID3D11DepthStencilState*    m_DepthEnableState;
	ID3D11DepthStencilState*    m_DepthDisableState;
	DirectDevice*				m_device;
	ID3D11RasterizerState*		m_RSWireFrame;
	ID3D11RasterizerState*		m_RSNormal;
	ID3D11BlendState*			m_TransparentBS;
	ID3D11BlendState*			m_offBS;
	ID3D11Texture2D*			m_DepthTexture;
	ID3D11DepthStencilView*		m_DepthStencilView;
	ID3D11SamplerState*			samplerState_;
};

#endif