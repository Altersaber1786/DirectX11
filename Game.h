#ifndef _GAME_H_
#define _GAME_H_


#include "GameObject.h"
#include "Camera.h"
#include "GraphicRenderer.h"
#include "GameInput.h"
#include <vector>
#include "GameTime.h"
class Game: public GameInput
{
public:
	bool Initialize(HINSTANCE hInst,HWND hwnd);
	bool LoadContent();
	void Update();
	void Render();
	Game();
	~Game();
	GraphicRenderer* m_Graphic;

protected:
	GameObject*		Player;
	GameObject*		Player2;
	GameObject*		Player3;
	GameObject*		Player4;
	Camera*			cam_1;

private:
	int totalMaterials;
	int totalModels;
	
	GameTime*		m_timeCounter;
};

#endif