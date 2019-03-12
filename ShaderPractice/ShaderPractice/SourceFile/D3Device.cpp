#pragma once
#include"stdafx.h"
#include"D3Device.h"
#include"MeshObjectFactory.h"

#define D3DX_PI ((FLOAT) 3.141592654f) 
#define D3DXToRadian( degree ) ((degree) * (D3DX_PI / 180.0f))
#define D3DXToDegree( radian ) ((radian) * (180.0f / D3DX_PI))

//定数定義
#define WINDOW_WIDTH 1980 //ウィンドウ幅
#define WINDOW_HEIGHT 1080 //ウィンドウ高さ



D3DRenderer::D3DRenderer()
{

}

D3DRenderer::~D3DRenderer()
{
	Release();
}

//デバイス作成
HRESULT D3DRenderer::CreateDevice(HWND hWnd)
{
	HRESULT hr = S_OK;
	UINT Flags = 0;
#ifdef _DEBUG
	//Flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL pLevels = { D3D_FEATURE_LEVEL_11_0 };
	D3D_FEATURE_LEVEL Level;

	DXGI_SWAP_CHAIN_DESC ScDesc;

	//SetRect(&rect, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	rect.right = rect.right - rect.left;
	rect.bottom = rect.bottom - rect.top;
	rect.top = 0;
	rect.left = 0;
	::ZeroMemory(&ScDesc, sizeof(ScDesc));
	ScDesc.BufferCount = 1;
	ScDesc.BufferDesc.Width = rect.right;
	ScDesc.BufferDesc.Height = rect.bottom;
	ScDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	ScDesc.BufferDesc.ScanlineOrdering= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	ScDesc.BufferDesc.Scaling= DXGI_MODE_SCALING_UNSPECIFIED;
	ScDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	ScDesc.OutputWindow = hWnd;
	ScDesc.SampleDesc.Count = 1;
	ScDesc.SampleDesc.Quality = 0;
	ScDesc.Windowed = TRUE;
	ScDesc.Flags= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	//デバイスと色々設定
	hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		Flags,
		&pLevels,
		1,
		D3D11_SDK_VERSION,
		&ScDesc,
		m_pSwapChain.GetAddressOf(),
		m_pDevice.GetAddressOf(),
		&Level,
		m_pDevContext.GetAddressOf());

	HDC hdc=GetDC(hWnd);
	int fr=GetDeviceCaps(hdc, VREFRESH);


	if (FAILED(hr))
		return hr;
	//バックバッファ設定		
	ID3D11Texture2D* pBackBuffer;

	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return hr;

	hr = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, m_pRenderTrgView.GetAddressOf());
	if (FAILED(hr))
		return hr;

	pBackBuffer->Release();

	//深度バッファ設定
	D3D11_TEXTURE2D_DESC TexDesc;
	::ZeroMemory(&TexDesc, sizeof(TexDesc));

	TexDesc.Width = ScDesc.BufferDesc.Width;
	TexDesc.Height = ScDesc.BufferDesc.Height;
	TexDesc.MipLevels = 1;
	TexDesc.ArraySize = 1;
	TexDesc.Format =  DXGI_FORMAT_D24_UNORM_S8_UINT;
	TexDesc.SampleDesc = ScDesc.SampleDesc;
	TexDesc.Usage = D3D11_USAGE_DEFAULT;
	TexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TexDesc.CPUAccessFlags = 0;
	TexDesc.MiscFlags = 0;
	hr = m_pDevice->CreateTexture2D(&TexDesc, NULL, m_pDepthStencilTex.GetAddressOf());
	if (FAILED(hr))
		return hr;

	D3D11_DEPTH_STENCIL_VIEW_DESC DsvDesc;
	::ZeroMemory(&DsvDesc, sizeof(DsvDesc));
	DsvDesc.Format = TexDesc.Format;
	DsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DsvDesc.Texture2D.MipSlice = 0;
	hr = m_pDevice->CreateDepthStencilView(m_pDepthStencilTex.Get(),&DsvDesc, m_pDepthStencilView.GetAddressOf());
	if (FAILED(hr))
		return hr;

	//ビューポート設定
	D3D11_VIEWPORT Vp;
	Vp.Height = WINDOW_HEIGHT;
	Vp.Width = WINDOW_WIDTH;
	Vp.MinDepth = 0.0f;
	Vp.MaxDepth = 1.0f;
	Vp.TopLeftX = 0;
	Vp.TopLeftY = 0;
	m_pDevContext->RSSetViewports(1, &Vp);
	
	//ラスタライザ設定
	D3D11_RASTERIZER_DESC RDesc;
	ZeroMemory(&RDesc, sizeof(RDesc));
	RDesc.CullMode = D3D11_CULL_NONE;
	RDesc.FillMode = D3D11_FILL_SOLID;

	ID3D11RasterizerState* pRSState = NULL;
	hr = m_pDevice->CreateRasterizerState(&RDesc, &pRSState);
	if (FAILED(hr))
		return hr;

	m_pDevContext->RSSetState(pRSState);
	pRSState->Release();
	pRSState = NULL;

	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory(&BlendDesc, sizeof(BlendDesc));
	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = m_pDevice->CreateBlendState(&BlendDesc, m_pBlendState.GetAddressOf());

	if (FAILED(hr))
		return hr;

	CreateVShader("VertexShader.hlsl");
	CreatePShader("PixelShader.hlsl");
	return hr;
}

//デバイスの破棄
void D3DRenderer::Release()
{
	if (m_pDevContext)
	{
		m_pDevContext->ClearState();
		m_pDevContext.Reset();
	}

	if (m_pRenderTrgView != NULL)
	{
		m_pRenderTrgView.Reset();
	}

	if (m_pSwapChain != NULL)
	{
		m_pSwapChain.Reset();
	}

	if (m_pDepthStencilTex != NULL)
	{
		m_pDepthStencilTex.Reset();
	}

	/*if (m_pVertexShader != NULL)
	{
		m_pVertexShader->Release();
		m_pVertexShader = NULL;
	}

	if (m_pPixelShader != NULL)
	{
		m_pPixelShader->Release();
		m_pPixelShader = NULL;
	}*/
}

//レンダリング
void D3DRenderer::RenderStart()
{
	if (m_pDevice == NULL) return;
	if (m_pDevContext == NULL) return;
	if (m_pSwapChain == NULL) return;
	if (m_pRenderTrgView == NULL) return;

	float ClearColor[4] = { 0.0f,0.0f,1.0f,1.0f };
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//画面更新
	m_pDevContext->ClearRenderTargetView(m_pRenderTrgView.Get(), ClearColor);
	m_pDevContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_pDevContext->OMSetBlendState(m_pBlendState.Get(), blendFactor, 0xffffffff);
	m_pDevContext->OMSetRenderTargets(1, m_pRenderTrgView.GetAddressOf(), m_pDepthStencilView.Get());
}

void D3DRenderer::RenderEnd()
{
	//if (m_pDevice == NULL) return;
	//if (m_pDevContext == NULL) return;
	//if (m_pSwapChain == NULL) return;
	//if (m_pRenderTrgView == NULL) return;

	//float ClearColor[4] = { 0.0f,0.0f,1.0f,1.0f };
	//float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	////画面更新
	//m_pDevContext->ClearRenderTargetView(m_pRenderTrgView.Get(), ClearColor);
	//m_pDevContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//
	//XMVECTOR Light= XMVectorSet(1, -1, 0, 0);
	///*XMMATRIX LightProj, LightView;
	//LightProj=XMMatrixPerspectiveFovLH(D3DXToRadian(45.0), WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
	//LightProj._11 = WINDOW_WIDTH / WINDOW_HEIGHT;
	//LightView=XMMatrixLookAtLH(XMLoadFloat3(&XMFLOAT3(0,1.0,0)), XMLoadFloat3(&XMFLOAT3(0, -1.0, 0.0)), XMLoadFloat3(&XMFLOAT3(0, 0.0, 1.0)));*/
	//
	//XMMATRIX ProjMat = XMMatrixIdentity();
	//XMMATRIX ViewMat = XMMatrixIdentity();

	////透視射影変換行列
	//if (m_pCamera != NULL)
	//{
	//	ProjMat = m_pCamera->GetProjMat();
	//	ViewMat = m_pCamera->GetViewMatW();
	//}

	//m_pDevContext->OMSetBlendState(m_pBlendState.Get(), blendFactor, 0xffffffff);
	//m_pDevContext->OMSetRenderTargets(1, m_pRenderTrgView.GetAddressOf(), m_pDepthStencilView.Get());

	//for (int i = 0; i < GObj.size(); i++)
	//{
	//	if (GObj[i] == NULL)
	//		continue;
	//	if (!(GObj[i]->IsEnable) || !(GObj[i]->IsAppear))
	//		continue;

	//	//デバイスコンテキストに値渡し
	//	//ワールド座標
	//	XMMATRIX WorldMat;
	//	WorldMat = GObj[i]->m_WorldMat.GetMat();

	//	//姿勢
	//	D3D11_MAPPED_SUBRESOURCE pData;
	//	if (SUCCEEDED(m_pDevContext->Map(MatBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	//	{
	//		ShaderGrobal SG;
	//		SG.ProjMat = XMMatrixTranspose(ProjMat);
	//		SG.ViewMat = XMMatrixTranspose(ViewMat);
	//		SG.WorldMat = XMMatrixTranspose(WorldMat);
	//		XMStoreFloat4(&SG.LightVec, Light);

	//		memcpy_s(pData.pData, pData.RowPitch, (void*)&SG, sizeof(ShaderGrobal));
	//		m_pDevContext->Unmap(MatBuffer, 0);
	//	}

	//	m_pDevContext->VSSetConstantBuffers(0, 1, &MatBuffer);
	//	m_pDevContext->PSSetConstantBuffers(0, 1, &MatBuffer);

	//	//プリミティブトポロジーのセット
	//	m_pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//	//頂点
	//	UINT Stride = sizeof(MyVertex);
	//	UINT Offset = 0;
	//	m_pDevContext->IASetVertexBuffers(0, 1, GObj[i]->GetMeshObject().m_pVertexBuffer.GetAddressOf(), &Stride, &Offset);

	//	//マテリアル関連
	//	for (int j = 0; j < GObj[i]->GetMeshObject().GetMaterialNum(); j++)
	//	{
	//		//シェーダ
	//		m_pDevContext->VSSetShader(m_pDefVShader, NULL, 0);
	//		m_pDevContext->PSSetShader(m_pDefPShader, NULL, 0);
	//		//頂点インデックス
	//		m_pDevContext->IASetIndexBuffer(GObj[i]->GetMeshObject().m_pIndexBuffer[j].Get(), DXGI_FORMAT_R32_UINT, 0);
	//		//マテリアル
	//		D3D11_MAPPED_SUBRESOURCE MaterialData;
	//		if (SUCCEEDED(m_pDevContext->Map(GObj[i]->GetMeshObject().m_pMaterialBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MaterialData)))
	//		{
	//			MyLambert Lambert;
	//			Lambert = GObj[i]->GetMeshObject().GetMaterial(j);
	//			memcpy_s(MaterialData.pData, MaterialData.RowPitch, &Lambert, sizeof(MyLambert));
	//			m_pDevContext->Unmap(GObj[i]->GetMeshObject().m_pMaterialBuffer.Get(), 0);
	//		}
	//		
	//		m_pDevContext->PSSetConstantBuffers(0, 1, GObj[i]->GetMeshObject().m_pMaterialBuffer.GetAddressOf());
	//		//テクスチャサンプラ
	//		m_pDevContext->PSSetSamplers(0, 1, GObj[i]->GetMeshObject().m_pSampleLiner.GetAddressOf());
	//		//テクスチャ
	//		if (GObj[i]->GetMeshObject().GetTexture(j) != NULL)
	//		{
	//			m_pDevContext->PSSetShaderResources(0, 1, GObj[i]->GetMeshObject().GetTexture(j));
	//		}

	//		m_pDevContext->DrawInstanced(GObj[i]->GetMeshObject().GetIndexNum(j), 1, 0, 0);
	//	}
	//}

	////Zバッファ無効
	//m_pDevContext->OMSetRenderTargets(1, m_pRenderTrgView.GetAddressOf(), NULL);

	////スプライト描画
	//for (int i = 0; i < Sprites.size(); i++)
	//{
	//	if (Sprites[i]==NULL) continue;
	//	if (!(Sprites[i]->IsEnable) || !(Sprites[i]->IsAppear)) continue;

	//	//シェーダ登録
	//	m_pDevContext->VSSetShader(m_pDefVShader.Get(), NULL, 0);
	//	m_pDevContext->PSSetShader(m_pDefPShader.Get(), NULL, 0);
	//	//ワールド座標
	//	XMMATRIX WorldMat;
	//	WorldMat = Sprites[i]->m_WorldMat.GetMat();

	//	D3D11_MAPPED_SUBRESOURCE pData;
	//	if (SUCCEEDED(m_pDevContext->Map(MatBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	//	{
	//		ShaderGrobal SG;
	//		SG.ProjMat = XMMatrixTranspose(ProjMat);
	//		SG.ViewMat = XMMatrixTranspose(ViewMat);
	//		SG.WorldMat = XMMatrixTranspose(WorldMat);
	//		XMStoreFloat4(&SG.LightVec, Light);

	//		memcpy_s(pData.pData, pData.RowPitch, (void*)&SG, sizeof(ShaderGrobal));
	//		m_pDevContext->Unmap(MatBuffer, 0);
	//	}

	//	m_pDevContext->VSSetConstantBuffers(0, 1, &MatBuffer);
	//	m_pDevContext->PSSetConstantBuffers(0, 1, &MatBuffer);

	//	//プリミティブトポロジーのセット
	//	m_pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//	Sprites[i]->Draw(m_pDevContext.Get());
	//}

	//画面更新
 	m_pSwapChain->Present(0, 0);
}

HRESULT D3DRenderer::CreateVShader(LPCSTR Path)
{
	ID3D11VertexShader* pVertexShader;
	ID3DBlob* pCompiledShader;
	ID3DBlob* pError;
	ID3D11InputLayout* VertexLayout;

	char RelativePath[255] = "../ShaderPractice/Shader/";
	strcat_s(RelativePath, 255, Path);

	if (FAILED(D3DX11CompileFromFile(RelativePath, NULL, NULL, "main", "vs_5_0", 0, 0, NULL, &pCompiledShader, &pError, NULL)))
	{
		MessageBox(0, "頂点シェーダー読み込み失敗", NULL, MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pError);

	if (FAILED(m_pDevice->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, &m_pDefVShader)))
	{
		MessageBox(0, "頂点シェーダー読み込み失敗", NULL, MB_OK);
		return E_FAIL;
	}

	//頂点インプットレイアウトを定義 
	D3D11_INPUT_ELEMENT_DESC Layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = sizeof(Layout) / sizeof(Layout[0]);

	if (m_pDevice->CreateInputLayout(Layout, numElements, pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), &VertexLayout))
	{
		return E_FAIL;
	}

	m_pDevContext->IASetInputLayout(VertexLayout);
	
	SAFE_RELEASE(VertexLayout);
	SAFE_RELEASE(pCompiledShader);

	return S_OK;
}

HRESULT D3DRenderer::CreatePShader(LPCSTR Path)
{
	ID3DBlob* pCompiledShader;
	ID3DBlob* pError;

	char RelativePath[255] = "../ShaderPractice/Shader/";
	strcat_s(RelativePath, 255, Path);

	if (FAILED(D3DX11CompileFromFileA(RelativePath, NULL, NULL, "main", "ps_5_0", 0, 0, NULL, &pCompiledShader, &pError, NULL)))
	{
		MessageBox(0, "ピクセルシェーダー読み込み失敗", NULL, MB_OK);
		return E_FAIL;
	}

	SAFE_RELEASE(pError);
	if (FAILED(m_pDevice->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, &m_pDefPShader)))
	{
		MessageBox(0, "ピクセルシェーダー読み込み失敗", NULL, MB_OK);
		SAFE_RELEASE(pCompiledShader);
		return E_FAIL;
	}

	SAFE_RELEASE(pCompiledShader);

	return S_OK;
}

ID3D11Device* D3DRenderer::GetDevice()
{
	return m_pDevice.Get();
}

ID3D11DeviceContext* D3DRenderer::GetDevContext()
{
	return m_pDevContext.Get();
}