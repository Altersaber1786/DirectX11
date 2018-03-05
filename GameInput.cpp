#include "GameInput.h"



GameInput::GameInput()
{
}


BOOL CALLBACK EnumDevicesCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
{
	
	char result[MAX_PATH + 1];
	strcpy(result, pdidInstance->tszProductName);
	int len = static_cast<int>(strlen(result));
	result[len] = '\n';
	result[len + 1] = '\0';
	OutputDebugStringA(result);
	return DIENUM_CONTINUE;
}

bool GameInput::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&m_DirectInputObj, NULL);
	if (hr != DI_OK)
	{
		MessageBox(NULL, "Failed to create direct input object", "Error", 0);
		return false;
	}


	//Create and setting up keyboard device
	hr = m_DirectInputObj->CreateDevice(GUID_SysKeyboard, &m_KeyboardDevice, NULL);
	if (hr != DI_OK)
	{
		MessageBox(NULL, "Failed to create keyboard device", "Error", 0);
		return false;
	}
	//Data format
	hr = m_KeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	if (hr != DI_OK)
	{
		MessageBox(NULL, "Failed to set keyboard device data format", "Error", 0);
		return false;
	}
	//Cooperative level
	hr = m_KeyboardDevice->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (hr != DI_OK)
	{
		MessageBox(NULL, "Failed to set keyboard cooperative level", "Error", 0);
		return false;
	}
	//Accquire the keyboard
	hr = m_KeyboardDevice->Acquire();
	if( hr != DI_OK)
	{
		MessageBox(NULL, "Cannot acquire the keyboard device", "Error", 0);
		return false;
	}

	//Create and setting up mouse device
	hr = m_DirectInputObj->CreateDevice(GUID_SysMouse, &m_MouseDevice, NULL);
	if (hr != DI_OK)
	{
		MessageBox(NULL, "Failed to create mouse device", "Error", 0);
		return false;
	}
	//Data format
	hr = m_MouseDevice->SetDataFormat(&c_dfDIMouse);
	if (hr != DI_OK)
	{
		MessageBox(NULL, "Failed to set mouse data format", "Error", 0);
		return false;
	}

	//Cooperative level
	hr = m_MouseDevice->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (hr != DI_OK)
	{
		MessageBox(NULL, "Failed to set mouse cooperative level", "Error", 0);
		return false;
	}
	hr = m_MouseDevice->Acquire();
	if( hr  != DI_OK )
	{
		MessageBox(NULL, "Cannot acquire the mouse device", "Error", 0);
		return false;
	}
	hr = m_DirectInputObj->EnumDevices(DI8DEVCLASS_ALL, EnumDevicesCallback, NULL, DIEDFL_ATTACHEDONLY);
	if (hr != DI_OK)
	{

		MessageBox(NULL, "cannot enum devices", "Error", 0);
		return false;
	}
	MouseX = MouseY = MouseZ = 0;
	return true;
}
	
void GameInput::Update()
{
	
	HRESULT hr = m_KeyboardDevice->GetDeviceState(sizeof(m_KeyboardState), (LPVOID)&m_KeyboardState);
	
	memcpy(m_PrevKeyboardState, m_KeyboardState, sizeof(m_KeyboardState));

	hr = m_MouseDevice->GetDeviceState(sizeof(m_mouseState), (LPVOID)&m_mouseState);

	m_prevMouseState = m_mouseState;

}
void GameInput::Release()
{

	if (m_DirectInputObj != nullptr)
	{
		m_DirectInputObj->Release();
		m_DirectInputObj = nullptr;
	}

	if (m_KeyboardDevice != nullptr)
	{
		m_KeyboardDevice->Unacquire();
		m_KeyboardDevice->Release();
		m_KeyboardDevice = nullptr;
	}

	if (m_MouseDevice != nullptr)
	{
		m_MouseDevice->Unacquire();
		m_MouseDevice->Release();
		m_MouseDevice = nullptr;
	}
}
GameInput::~GameInput()
{

}
