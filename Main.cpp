#include "Game.h"

LRESULT CALLBACK WndProc(HWND, UINT , WPARAM, LPARAM);
HWND hwnd;
HINSTANCE hInstance;
static bool shouldResize = false;
bool RegisterWindowClass()
{
	//Register the window class
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = HBRUSH(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = "DX11BookWindowClass";
	if (!RegisterClassEx(&wndClass))
		return false;

	RECT rc = { 0, 0, 800, 640 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	hwnd = CreateWindowA("DX11BookWindowClass",
		"Blank Win32 Window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		NULL,
		NULL,
		hInstance,
		NULL);
	
}
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR cmdLine, int cmdShow)
{	
	if (!RegisterWindowClass())
	{
		return -1;
	};
	if (!hwnd) return -1;
	ShowWindow(hwnd, cmdShow);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(cmdLine);

	//Message
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	//Game 
	Game *game = new Game();
	if (!game->Initialize(hwnd))
	{
		MessageBox(hwnd, "Failed to initialize the game!", NULL, 0);
		delete game;
		return -1;
	};

	//Message processing
	int i = 0;
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (shouldResize)
		{
			game->m_device->swapChainSetNewSize(hwnd);
			shouldResize = false;
		}
		game->Update();
		game->Render();
	}
	delete game;
	return static_cast <int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct;
	HDC hDC;
	switch (message)
	{  

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &paintStruct);
		EndPaint(hWnd, &paintStruct);
		break;
	case WM_SIZE:
		shouldResize = true;
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}