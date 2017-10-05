#ifndef _GAME_H_
#define _GAME_H_


#include "GameObject.h"
#include "Camera.h"
#include "GraphicRenderer.h"
#include <vector>

class Game
{
public:
	bool Initialize(HWND hwnd);
	bool LoadContent();
	void Update();
	void Render();
	Game();
	~Game();
	void Relase();
	GraphicRenderer* m_Graphic;
private:
	int totalMaterials;
	int totalModels;
	GameObject*		Player;
	GameObject*		Player2;

	Camera*			cam_1;
	
};

#endif