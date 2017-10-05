#include "Game.h"


Game::Game()
{
	totalMaterials = 0;
}

bool Game::Initialize(HWND hwnd)
{
	

	m_Graphic = new GraphicRenderer();
	if (!m_Graphic->Initialize(hwnd))
		return false;
	m_Graphic->LoadModelList();
	
	LoadContent();

	return true;
}

bool Game::LoadContent()
{
	Player = new GameObject();
	Player->Initialize(XMFLOAT3(-5.0f, 1.0f, 19.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(1.0f, 1.0f, 1.0f));
	Player->modelIndex = 0;

	Player2 = new GameObject();
	Player2->Initialize(XMFLOAT3(4.0f, 0.0f, 17.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(1.0f, 1.0f, 1.0f));
	Player2->modelIndex = 0;
	cam_1 = new Camera();


	cam_1->Follow(Player);

	m_Graphic->LoadObject(Player);
	m_Graphic->LoadObject(Player2);
	m_Graphic->LoadCamera(cam_1);
	return true;
}

void Game::Update()
{
	Player->position_.x += 0.005;
	//Player->position_.y += 0.005;
	//Player->rotation_.y += 0.001;
	m_Graphic->Update();
}

void Game::Render()
{
	m_Graphic->Render();
}

Game::~Game()
{
	delete cam_1;
	cam_1 = 0;
	delete m_Graphic;
	m_Graphic = 0;
}
