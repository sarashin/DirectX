#include"stdafx.h"
#include"Application.h"

#define D3DX_PI ((FLOAT) 3.141592654f) 
#define D3DXToRadian( degree ) ((degree) * (D3DX_PI / 180.0f))

AppBase* pApp;

AppBase::AppBase() 
{
	pApp = this;
}

AppBase::~AppBase()
{}

//ウインドウのグローバルプロシージャ
LRESULT CALLBACK AppBase::GrobalWndProc(HWND hWnd,
	UINT Msg,
	WPARAM wParam,
	LPARAM lParam
)
{
	if (pApp)
	{
		vector<shared_ptr<Window>>* pWnd = pApp->GetWndPtr();
		int Size = pWnd->size();

		for (int i = 0; i < Size; i++)
		{
			Window* WndBase = (*pWnd)[i].get();
			if (WndBase)
				return WndBase->LocalWndProc(hWnd, Msg, wParam, lParam);
		}
	}
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

//ウインドウの定義
HRESULT AppBase::RegistWndClass()
{
	m_WndClass= { sizeof(WNDCLASSEX),
		CS_CLASSDC,
		GrobalWndProc,
		0L,
		0L,
		GetModuleHandle(NULL),
		NULL,
		NULL,
		NULL,
		NULL,
		"Window",
		NULL };

	if (RegisterClassEx(&m_WndClass) > 0)
		return S_OK;
	else
		return HRESULT(-1);
}

//メインウインドウ作成
HRESULT AppBase::CreateMainWindow()
{
	shared_ptr<Window> pWnd = make_shared<Window>();
	pWnd->Create(m_WndClass.hInstance, "MainWindow",NULL);
	m_pWndAry.push_back(pWnd);

	return S_OK;
}

//更新処理
void AppBase::Update()
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

//アプリケーションの動き
HRESULT AppBase::Run(HINSTANCE hInst)
{
	HRESULT hr = S_OK;
	m_hInstance = hInst;

	if(FAILED(this->RegistWndClass()))
	{
		hr = HRESULT(-1);
		return hr;
	}

	if(FAILED(this->CreateMainWindow()))
	{
		hr = HRESULT(-1);
		return hr;
	}

	if (!this->Init())
	{
		hr = HRESULT(-1);
		return hr;
	}

	this->Update();

	this->Release();
}

//インスタンスハンドル取得
HINSTANCE AppBase::GetHInstance()
{
	return m_hInstance;
}

//ウインドウクラスの取得
void AppBase::GetWindowClass(unique_ptr<WNDCLASSEX> WndClass)
{
	*WndClass = m_WndClass;
}

//ウインドウのポインタ取得
vector<shared_ptr<Window>>* AppBase::GetWndPtr()
{
	return &m_pWndAry;
}

Application::Application()
{
	pApp = this;
}

Application::~Application()
{

}

//ウインドウの定義
HRESULT Application::RegistWndClass()
{
	m_WndClass = { sizeof(WNDCLASSEX),
		CS_CLASSDC,
		GrobalWndProc,
		0L,
		0L,
		GetModuleHandle(NULL),
		NULL,
		NULL,
		NULL,
		NULL,
		"MainWindow",
		NULL };

	if (RegisterClassEx(&m_WndClass) > 0)
		return S_OK;
	else
		return HRESULT(-1);
}

//メインウインドウ作成
HRESULT Application::CreateMainWindow()
{
	shared_ptr<Window> pWnd = make_shared<Window>();
	pWnd->Create(m_WndClass.hInstance, m_WndClass.lpszClassName,NULL);
	m_pWndAry.push_back(pWnd);
	m_DxBase = make_unique<Content>(pWnd->GetWindow(), m_hInstance);

	return S_OK;
}

//初期化処理
bool Application::Init()
{
	shared_ptr<Window> DebugWindow = make_shared<Window>();
	DebugWindow->Create(m_WndClass.hInstance, m_WndClass.lpszClassName,m_pWndAry[0]->GetWindow());
	m_pWndAry.push_back(DebugWindow);
	return true;
}

//更新処理
void Application::Update()
{
	if (!m_DxBase)
		return;

	m_DxBase->Init();
	
	MSG msg;
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			m_DxBase->Release();
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_DxBase->Update();
		}
	}
}

//解放処理
void Application::Release()
{

}