#pragma once
#include"Mesh.h"
#include"Camera.h"
#include"MeshObject.h"
#include"SpriteObject.h"

class MeshObject;
class SpriteObject;

#define SAFE_RELEASE(x) if(x){x->Release();x=NULL;}

class D3DRenderer
{
private:
	//DirectXの変数
	ComPtr<ID3D11Device>			m_pDevice;
	ComPtr<ID3D11DeviceContext>		m_pDevContext;
	ComPtr<IDXGISwapChain>			m_pSwapChain;
	ComPtr<ID3D11RenderTargetView>	m_pRenderTrgView;
	ComPtr<ID3D11DepthStencilView>	m_pDepthStencilView;
	ComPtr<ID3D11Texture2D>			m_pDepthStencilTex;
	ComPtr<ID3D11BlendState>		m_pBlendState;
	
protected:
public:
	//デフォルトシェーダ
	ComPtr<ID3D11VertexShader>		m_pDefVShader;
	ComPtr<ID3D11PixelShader>		m_pDefPShader;

	D3DRenderer();
	~D3DRenderer();

	CRect rect;

	ID3D11Device*			GetDevice();
	ID3D11DeviceContext*	GetDevContext();

	HRESULT CreateVShader(LPCSTR Pass);
	HRESULT CreatePShader(LPCSTR Pass);
	HRESULT CreateDevice(HWND hWnd);
	void	Release();
	
	void RenderStart();
	void RenderEnd();
};

struct ShaderGlobal
{
	XMMATRIX WorldMat;
	XMMATRIX ViewMat;
	XMMATRIX ProjMat;
};