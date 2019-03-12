#include "stdafx.h"
#include "Application.h"
#pragma comment(lib,"d3d11.lib")

#define D3DX_PI ((FLOAT) 3.141592654f) 
#define D3DXToRadian( degree ) ((degree) * (D3DX_PI / 180.0f))
#define D3DXToDegree( radian ) ((radian) * (180.0f / D3DX_PI))

//定数定義
#define WINDOW_WIDTH  1980//ウィンドウ幅
#define WINDOW_HEIGHT 1080//ウィンドウ高さ
//グローバル変数
HWND hWnd = NULL;

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szStr, INT iCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	////ウインドウクラスの登録
	//WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L,
	//	GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
	//	"Window1", NULL };
	//RegisterClassEx(&wc);
	////タイトルバーとウインドウ枠の分を含めてウインドウサイズを設定
	//RECT rect;
	//SetRect(&rect, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	////AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	//rect.right = rect.right - rect.left;
	//rect.bottom = rect.bottom - rect.top;
	//rect.top = 0;
	//rect.left = 0;
	////ウインドウの生成
	//hWnd = CreateWindowA("Window1", "Hello DirectX11 World !!",
	//	WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rect.right, rect.bottom,
	//	NULL, NULL, wc.hInstance, NULL);

	//MSG msg;
	//ZeroMemory(&msg, sizeof(msg));

	//ShowWindow(hWnd, SW_SHOW);

	//while (msg.message != WM_QUIT)
	//{
	//	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	//	{
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//	}
	//	else
	//	{
	//		UpdateWindow(hWnd);
	//	}
	//}
	////終了
	//return 0;

	Application App;
	App.Run(hInst);
}

void Release()
{

}