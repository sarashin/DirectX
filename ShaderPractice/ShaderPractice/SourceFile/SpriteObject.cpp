#include "stdafx.h"
#include "SpriteObject.h"
#include "MeshObjectFactory.h"

SpriteObject::SpriteObject() : GameObject()
{
	IsAppear = true;
}


SpriteObject::~SpriteObject()
{
	Release();
}

void SpriteObject::SetMeshObject(shared_ptr<Sprite>& pSprite)
{
	m_pSprite=pSprite;
}

Sprite* SpriteObject::GetMeshObject()
{
	return m_pSprite.get();
}

void SpriteObject::Create(const char* TexPath)
{
	RECT SpriteSize;
	SpriteSize.top = -660.0;
	SpriteSize.bottom = 660.0;
	SpriteSize.left = -990.0;
	SpriteSize.right = 990.0;

	RECT SpriteRect;
	SpriteRect.top = 1.0;
	SpriteRect.bottom = 0.0;
	SpriteRect.left = 0.0;
	SpriteRect.right = 1.0f;

	MeshFactory::GetInstance().CreateSprite(TexPath,m_pSprite,SpriteSize,SpriteRect);
}

void SpriteObject::Update()
{
	
}

//描画
void SpriteObject::Draw(Camera Cam,ComPtr<ID3D11DeviceContext> pDevContext)
{
	if (pDevContext == NULL)
	{
		return;
	}

	//シェーダのセット
	pDevContext->VSSetShader(m_pSprite->m_pVShader.get(), NULL, 0);
	pDevContext->PSSetShader(m_pSprite->m_pPShader.get(), NULL, 0);

	//カメラ情報から位置を計算
	//カメラのワールド座標からカメラの位置を計算
	XMMATRIX TransMat, RotateMat;
	TransMat = Cam.GetWorldMat();
	TransMat = XMMatrixInverse(TransMat.r, TransMat);

	//ビュー行列からカメラの視点を計算
	XMVECTOR Quat;
	Quat = XMQuaternionRotationMatrix(Cam.GetViewMatL());
	RotateMat = XMMatrixRotationQuaternion(Quat);
	RotateMat = XMMatrixInverse(RotateMat.r, RotateMat);

	m_WorldMat.SetMat(RotateMat*TransMat);

	//シェーダへ姿勢とカメラ行列を渡す
	D3D11_MAPPED_SUBRESOURCE pData;
	if (SUCCEEDED(pDevContext->Map(m_pMatrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		ShaderGrobal SG;
		SG.ProjMat = XMMatrixTranspose(Cam.GetProjMat());
		SG.ViewMat = XMMatrixTranspose(Cam.GetViewMatW());
		SG.WorldMat = XMMatrixTranspose(m_WorldMat.GetMat());

		memcpy_s(pData.pData, pData.RowPitch, (void*)&SG, sizeof(ShaderGrobal));
		pDevContext->Unmap(m_pMatrixBuffer.Get(), 0);
	}

	pDevContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);

	//プリミティブトポロジーのセット
	pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//シェーダへ頂点情報を渡す
	UINT Stride = sizeof(MyVertex);
	UINT Offset = 0;
	//頂点情報
	pDevContext->IASetVertexBuffers(0, 1, m_pSprite->m_pVertexBuffer.GetAddressOf(), &Stride, &Offset);
	//頂点インデックス
	pDevContext->IASetIndexBuffer(m_pSprite->m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//シェーダへマテリアルを渡す
	D3D11_MAPPED_SUBRESOURCE MaterialData;
	if (SUCCEEDED(pDevContext->Map(m_pSprite->m_pMaterialBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MaterialData)))
	{
		MyLambert Lambert;
		Lambert = m_pSprite->GetMaterial();
		memcpy_s(MaterialData.pData, MaterialData.RowPitch, &Lambert, sizeof(MyLambert));
		pDevContext->Unmap(m_pSprite->m_pMaterialBuffer.Get(), 0);
	}

	pDevContext->PSSetConstantBuffers(0, 1, m_pSprite->m_pMaterialBuffer.GetAddressOf());
	pDevContext->PSSetSamplers(0, 1, m_pSprite->m_pSampleLiner.GetAddressOf());

	//シェーダへテクスチャリソースを渡す
	if (m_pSprite->GetTexture() != NULL)
	{
		pDevContext->PSSetShaderResources(0, 1, m_pSprite->GetTexture());
	}

	//描画
	pDevContext->DrawInstanced(m_pSprite->GetMesh().PolygonCount, 1, 0, 0);
}

void SpriteObject::CopyObject(SpriteObject** ppObject)
{}

void SpriteObject::Release()
{
	m_pSprite.reset();
}