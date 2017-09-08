#ifndef _GAME_H_
#define _GAME_H_

#include "DirectDevice.h"
#include "Sprite.h"
#include "Camera.h"
class Game
{
public:
	bool Initialize(HWND hwnd);
	bool LoadContent();
	void Update();
	void Render();
	Game();
	~Game();
	DirectDevice*	m_device;

private:
	Camera*			cam_1;
	Sprite*			obj_1; 
	Sprite*			obj_2;
};

#endif