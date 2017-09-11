#include "Game.h"


Game::Game()
{
}

bool Game::Initialize(HWND hwnd)
{
	m_device = new DirectDevice();
	if (!m_device->Initialize(hwnd, true))
	{
		return false;
	};

	obj_1 = new Sprite();
	if (!obj_1->Initialize(
		XMFLOAT3(-5.0f, 1.0f, 16.0f),
		XMFLOAT3(0.0f, 1.0f, 1.0f),
		XMFLOAT3(1.0f, 1.0f, 1.0f),
		m_device, (LPCWSTR)L"TextureMap.fx", L"Renge.DDS", "Sphere.obj"))
	{
		return false;
	};

	
	cam_1 = new Camera();
	if (!cam_1->Initialize(m_device))
	{
		return false;
	}
	cam_1->Follow(obj_1);
	return true;
}

bool Game::LoadContent()
{
	////////
	return true;
}

void Game::Update()
{
	obj_1->rotation_.x += 0.001;
	obj_1->rotation_.y += 0.001;
	obj_1->Update();
	cam_1->Update(m_device);
}

void Game::Render()
{
	m_device->Clear();
	obj_1->Render(m_device);
	m_device->Present();
}

Game::~Game()
{
		delete cam_1;
		delete obj_1;
		delete obj_2;
		delete m_device;
}
