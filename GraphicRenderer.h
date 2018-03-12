#ifndef GRAPHIC_RENDERER_H
#define GRAPHIC_RENDERER_H

#include "LightShader.h"
#include "GameMaterial.h"
#include "GameModel.h"
#include "Camera.h"

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

	void BeginScene();
	void EndScene();
	float clearColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
	Camera*		camera;

	UINT		totalMaterials;
	UINT		totalModels;
	UINT		windowWidth;
	UINT		windowHeight;


	std::vector<GameMaterial*>	MaterialList;
	std::vector<GameModel*>		ModelList;

	LightShader*				m_deferredLighting;
	ID3D11RenderTargetView*		m_backBufferTarget;

	ID3D11DepthStencilState*    m_DepthEnableState;
	ID3D11DepthStencilState*    m_DepthDisableState;
	DirectDevice*				m_device;
	ID3D11RasterizerState*		m_RSWireFrame;
	ID3D11RasterizerState*		m_RSNormal;

};

#endif