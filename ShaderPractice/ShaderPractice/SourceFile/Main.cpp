#include "stdafx.h"
#include "Application.h"
#pragma comment(lib,"d3d11.lib")

#define D3DX_PI ((FLOAT) 3.141592654f) 
#define D3DXToRadian( degree ) ((degree) * (D3DX_PI / 180.0f))
#define D3DXToDegree( radian ) ((radian) * (180.0f / D3DX_PI))

//�萔��`
#define WINDOW_WIDTH  1980//�E�B���h�E��
#define WINDOW_HEIGHT 1080//�E�B���h�E����
//�O���[�o���ϐ�
HWND hWnd = NULL;

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szStr, INT iCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	////�E�C���h�E�N���X�̓o�^
	//WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L,
	//	GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
	//	"Window1", NULL };
	//RegisterClassEx(&wc);
	////�^�C�g���o�[�ƃE�C���h�E�g�̕����܂߂ăE�C���h�E�T�C�Y��ݒ�
	//RECT rect;
	//SetRect(&rect, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	////AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	//rect.right = rect.right - rect.left;
	//rect.bottom = rect.bottom - rect.top;
	//rect.top = 0;
	//rect.left = 0;
	////�E�C���h�E�̐���
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
	////�I��
	//return 0;

	Application App;
	App.Run(hInst);
}

void Release()
{

}