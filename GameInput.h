#ifndef GAME_INPUT
#define GAME_INPUT


#include "DirectDevice.h"
#include <dinput.h>
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif
#define KEY_DOWN(buffer, key) (buffer[key]&0x80)
#define BUTTON_DOWN(name, key) (name.rgbButtons[key]&0x80)

class GameInput
{
public:
	GameInput(); 
	~GameInput();
	
protected:
	int i = 3;
	virtual void Update();
	void Release();
	bool Initialize(HINSTANCE hInstance, HWND hwnd);
	char m_KeyboardState[256];
	char m_PrevKeyboardState[256];
	LPDIRECTINPUT8 m_DirectInputObj;
	LPDIRECTINPUTDEVICE8	m_KeyboardDevice;
	LPDIRECTINPUTDEVICE8	m_MouseDevice;
	DIMOUSESTATE			m_mouseState;
	DIMOUSESTATE			m_prevMouseState;
	LONG					MouseX;
	LONG					MouseY;
	LONG					MouseZ;
};
#endif