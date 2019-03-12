#include"stdafx.h"
#include "Sprite.h"



Sprite::Sprite()
{
	
}

Sprite::Sprite(const Sprite& sprite)
{
	this->m_Mesh = sprite.m_Mesh;
	this->m_pTexture.Attach(sprite.m_pTexture.Get());
	this->m_Material = sprite.m_Material;
}

Sprite::Sprite(MyMesh Mesh, ID3D11ShaderResourceView* pTexture)
{
	m_Mesh = make_shared<MyMesh>(Mesh);
	m_pTexture.Attach(pTexture);
}

Sprite::~Sprite()
{
}

ID3D11ShaderResourceView** Sprite::GetTexture()
{
	if(m_pTexture!=NULL)
		return m_pTexture.GetAddressOf();

		return NULL;
}

int Sprite::GetVertCount()
{
	return m_Mesh->VertexCount;
}

int Sprite::GetIndexCount(int IndexNum)
{
	return m_Mesh->PolygonIndex.size();
}

MyMesh Sprite::GetMesh()
{
	return *m_Mesh;
}

MyLambert Sprite::GetMaterial()
{
	return m_Material;
}

//頂点取得
//MyVertex Sprite::GetVertex()
//{
//	return m_Mesh->Vertex[0];
//}

//メッシュのセット
void Sprite::SetMesh(MyMesh pMesh)
{
	*m_Mesh = pMesh;
}

//マテリアルのセット
void Sprite::SetMaterial(MyLambert Material)
{
	m_Material=Material;
}

//テクスチャのセット
void Sprite::SetTexture(ID3D11ShaderResourceView* SRView)
{
	if (SRView == NULL)
	{
		return;
	}

	m_pTexture.Attach(SRView);
}

//インデックスバッファ作成
void Sprite::CreateIndexBuffer(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(int)*GetIndexCount(0);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitIBData;
	InitIBData.pSysMem = &m_Mesh->PolygonIndex[0];
	InitIBData.SysMemPitch = 0;
	InitIBData.SysMemSlicePitch = 0;
	pDevice->CreateBuffer(&bd, &InitIBData, &m_pIndexBuffer);
}

//頂点バッファ作成
void Sprite::CreateVertBuffer(ID3D11Device* pDevice)
{
	MyVertex Vert;

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MyVertex) * m_Mesh->VertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitVBData;
	//InitVBData.pSysMem = &m_Mesh->
	InitVBData.SysMemPitch = 0;
	InitVBData.SysMemSlicePitch = 0;

	pDevice->CreateBuffer(&bd, &InitVBData, &m_pVertexBuffer);
}

//マテリアルバッファ作成
void Sprite::CreateMaterialBuffer(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC MBDesc;
	MBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MBDesc.ByteWidth = sizeof(MyLambert);
	MBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MBDesc.MiscFlags = 0;
	MBDesc.StructureByteStride = 0;
	MBDesc.Usage = D3D11_USAGE_DYNAMIC;

	pDevice->CreateBuffer(&MBDesc, NULL, &m_pMaterialBuffer);
}

void Sprite::Release()
{
	m_pIndexBuffer.Reset();
	m_pMaterialBuffer.Reset();
	m_pVertexBuffer.Reset();
	m_pSampleLiner.Reset();
	m_pTexture.Reset();
}

void Sprite::Draw(ID3D11DeviceContext* DeviceContext)
{
	if (DeviceContext == NULL)
	{
		return;
	}


	//m_WorldMat = ;

	/*D3D11_MAPPED_SUBRESOURCE pData;
	if (SUCCEEDED(DeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
	ShaderGrobal SG;
	SG.ProjMat = XMMatrixTranspose(ProjMat);
	SG.ViewMat = XMMatrixTranspose(ViewMat);
	SG.WorldMat = XMMatrixTranspose(m_WorldMat);
	SG.Eye = *EyeVec;
	SG.LightDir = *LightVec;

	memcpy_s(pData.pData, pData.RowPitch, (void*)&SG, sizeof(ShaderGrobal));
	DeviceContext->Unmap(m_pMatrixBuffer, 0);
	}*/

	/*DeviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);
	DeviceContext->PSSetConstantBuffers(0, 1, &m_pMatrixBuffer);*/

	UINT Stride = sizeof(MyVertex);
	UINT Offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &Stride, &Offset);

	/*D3D11_MAPPED_SUBRESOURCE BoneData;
	if (SUCCEEDED(Device->Map(m_pAnimContainer->m_BoneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &BoneData)))
	{
	XMMATRIX* Mat=new XMMATRIX;
	m_pAnimContainer->GetAnimMat(&Mat[0]);
	memcpy_s(BoneData.pData, BoneData.RowPitch, &Mat, sizeof(Mat)/sizeof(XMMATRIX));
	Device->Unmap(m_pAnimContainer->m_BoneBuffer, 0);
	delete Mat;
	Mat = NULL;
	}*/

	//Device->VSSetConstantBuffers(1, 1, &m_pAnimContainer->m_BoneBuffer);

	DeviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	D3D11_MAPPED_SUBRESOURCE MaterialData;
	if (SUCCEEDED(DeviceContext->Map(m_pMaterialBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MaterialData)))
	{
		MyLambert Lambert;
		Lambert = m_Material;
		memcpy_s(MaterialData.pData, MaterialData.RowPitch, &Lambert, sizeof(MyLambert));
		DeviceContext->Unmap(m_pMaterialBuffer.Get(), 0);
	}

	DeviceContext->VSSetConstantBuffers(1, 1, &m_pMaterialBuffer);
	DeviceContext->PSSetSamplers(0, 1, &m_pSampleLiner);
	DeviceContext->PSSetShaderResources(0, 1, m_pTexture.GetAddressOf());

	DeviceContext->DrawInstanced(m_Mesh->PolygonCount * 3, 1, 0, 0);
	
}