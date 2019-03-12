#include "stdafx.h"
#include "MeshObject.h"
#include"D3Device.h"
#include"MeshObjectFactory.h"


MeshObject::MeshObject() : GameObject()
{
	IsAppear = true;
}

MeshObject::MeshObject(MyMeshContainer Mesh) : GameObject()
{
	m_Mesh=Mesh;
	IsAppear = true;
}

MeshObject::MeshObject(MyMeshContainer Mesh,Transform Trans) : GameObject(Trans)
{
	m_Mesh=Mesh;
	IsAppear = true;
}

MeshObject::~MeshObject()
{
	Release();
}

////メッシュのセット
//void MeshObject::SetMeshObject(MyMeshData Mesh)
//{
//	m_pMeshObject = Mesh;
//}
//
////メッシュ取得
//MyMeshData& MeshObject::GetMeshObject()
//{
//	return m_pMeshObject;
//}

void MeshObject::Create(const char* Path,ComPtr<ID3D11Device> pDevice)
{
	MyAnimationContainer Con;
	MeshFactory::GetInstance().CreateMeshObject(Path,m_Mesh,Con);

	//姿勢のバッファ作成
	D3D11_BUFFER_DESC MatBDesc;
	MatBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MatBDesc.ByteWidth = sizeof(ShaderGrobal);
	MatBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MatBDesc.MiscFlags = 0;
	MatBDesc.StructureByteStride = 0;
	MatBDesc.Usage = D3D11_USAGE_DYNAMIC;

	pDevice->CreateBuffer(&MatBDesc, NULL, &m_pMatBuffer);
}

//描画
void MeshObject::Draw(Camera ProjCam, vector<Light> ProjLight,ComPtr<ID3D11DeviceContext> pDevContext)
{
	if (pDevContext == NULL)
		return;


	//シェーダへ渡す姿勢データ
	D3D11_MAPPED_SUBRESOURCE pData;
	if (SUCCEEDED(pDevContext->Map(m_pMatBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		ShaderGrobal SG;
		SG.ProjMat = XMMatrixTranspose(ProjCam.GetProjMat());
		SG.ViewMat = XMMatrixTranspose(ProjCam.GetViewMatW());
		SG.WorldMat = XMMatrixTranspose(m_WorldMat.GetMat());

		memcpy_s(pData.pData, pData.RowPitch, (void*)&SG, sizeof(ShaderGrobal));
		pDevContext->Unmap(m_pMatBuffer.Get(), 0);
	}

	pDevContext->VSSetConstantBuffers(0, 1, m_pMatBuffer.GetAddressOf());

	int Size = m_Mesh.GetMeshCount();
	UINT Offset = 0;
	UINT Stride = sizeof(MyVertex);
	pDevContext->IASetVertexBuffers(0, 1, m_Mesh.GetMesh(0).m_pVertexBuffer.GetAddressOf(), &Stride, &Offset);

	//メッシュごとに描画
	for (int i = 0; i < Size; i++)
	{
		MyMeshData ThisMesh = m_Mesh.GetMesh(i);

		pDevContext->VSSetShader(ThisMesh.m_pVShader.Get(), NULL, 0);
		pDevContext->PSSetShader(ThisMesh.m_pPShader.Get(), NULL, 0);

		pDevContext->IASetIndexBuffer(ThisMesh.m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		D3D11_MAPPED_SUBRESOURCE MaterialData;
		if (SUCCEEDED(pDevContext->Map(ThisMesh.m_pMaterialBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MaterialData)))
		{
			MyLambert Lambert;
			Lambert = ThisMesh.GetMaterial();
			memcpy_s(MaterialData.pData, MaterialData.RowPitch, &Lambert, sizeof(MyLambert));
			pDevContext->Unmap(ThisMesh.m_pMaterialBuffer.Get(), 0);
		}

		pDevContext->PSSetConstantBuffers(0, 1, ThisMesh.m_pMaterialBuffer.GetAddressOf());
		pDevContext->PSSetSamplers(0, 1, ThisMesh.m_pSampleLiner.GetAddressOf());
		pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		int TexSize = ThisMesh.GetTexCount();

		for (int j = 0; j < TexSize; j++)
		{
			if (ThisMesh.GetTexture(j) != NULL)
			{
				pDevContext->PSSetShaderResources(0, 1, ThisMesh.GetTexture(j).GetAddressOf());
			}
		}

		pDevContext->DrawInstanced(ThisMesh.GetPolyIndexCount(), 1, 0, 0);
	}
}

void MeshObject::Update()
{
	
}

//オブジェクトのコピー
void MeshObject::CopyObject(shared_ptr<MeshObject>& ppObjects)
{
	
}

void MeshObject::Release()
{
	m_pParentObject.reset();
	m_pChildObject.reset();
}