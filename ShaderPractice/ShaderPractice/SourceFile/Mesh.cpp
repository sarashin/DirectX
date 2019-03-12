#include"stdafx.h"
#include"Mesh.h"
#include"D3Device.h"

MyMesh::MyMesh()
{
	m_pVertex = make_shared<vector<MyVertex>>();
}

MyMesh::MyMesh(shared_ptr<vector<MyVertex>> pVertex)
{
	m_pVertex = pVertex;
	VertexCount = m_pVertex->size();
}

MyMesh::MyMesh(vector<XMFLOAT4> Positions, vector<XMFLOAT3> Normals, vector<XMFLOAT2> UVs, vector<int> Index)
{
	
	PolygonIndex = Index;
	VertexCount = Positions.size();
	PolygonCount = Index.size() / 3;
	PolygonVertexCount = Index.size();
}

MyMesh::~MyMesh()
{}

//void MyMesh::SetIndex(vector<int> IndexNum, int IndexCount)
//{
//	vector<int> Index;
//	Index.resize(IndexNum.size());
//	Index = IndexNum;
//	//PolygonIndex.push_back(Index);
//}
//
//void MyMesh::SetIndex(int* Index, int IndexCount)
//{
//	PolygonIndex.resize(IndexCount);
//
//	for (int i = 0; i < IndexCount; i++)
//	{
//		for (int j = 0; j < 3; j++)
//		{
//			//PolygonIndex[i][j] = Index[i];
//		}
//	}
//
//	PolygonCount += IndexCount;
//}


MyMeshData::MyMeshData()
{
	
}

MyMeshData::MyMeshData(MyMesh pMesh, MyLambert Lambert, vector<ComPtr<ID3D11ShaderResourceView>> pTex)
{
	m_Mesh = make_shared<MyMesh>();

	*m_Mesh = pMesh;
	m_Material = Lambert;
	m_pTexture = pTex;
	
}

MyMeshData::~MyMeshData()
{
	Release();
}

int MyMeshData::GetVertCount()
{
	return m_Mesh->m_pVertex->size();
}

int MyMeshData::GetIndexCount()
{
	return m_Mesh->PolygonIndex.size();
}

int MyMeshData::GetPolyIndexCount()
{
	return m_Mesh->PolygonVertexCount;
}

int MyMeshData::GetTexCount()
{
	return m_pTexture.size();
}

shared_ptr<MyMesh> MyMeshData::GetMesh()
{
	return m_Mesh;
}

MyLambert MyMeshData::GetMaterial()
{
	return m_Material;
}

//頂点取得
//vector<MyVertex> MyMeshData::GetVertex()
//{
//	return Vertex;
//}


ComPtr<ID3D11ShaderResourceView> MyMeshData::GetTexture(int Index)
{
	if (Index < m_pTexture.size())
		return m_pTexture[Index];
	else
		return NULL;
}


XMFLOAT4 MyMeshData::GetVertPos(int Index)
{
	return m_Mesh->m_pVertex->at(Index).Position;
}


vector<int> MyMeshData::GetIndex()
{
	return m_Mesh->PolygonIndex;
}

//メッシュのセット
void MyMeshData::SetMesh(shared_ptr<MyMesh> pMesh)
{
	m_Mesh=pMesh;
}

//マテリアルのセット
void MyMeshData::SetMaterial(MyLambert Material)
{
	m_Material=Material;
}

//テクスチャのセット
void MyMeshData::SetTexture(vector<ComPtr<ID3D11ShaderResourceView>> SRView)
{
	m_pTexture=SRView;
}

void MyMeshData::AddTexture(ComPtr<ID3D11ShaderResourceView> pSRView)
{
	if (pSRView)
		return;

	m_pTexture.push_back(pSRView);
}

//インデックスバッファ作成
void MyMeshData::CreateIndexBuffer(ComPtr<ID3D11Device> pDevice)
{
	if (pDevice == NULL)
	{
		return;
	}

	m_pIndexBuffer.Reset();
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(int)*m_Mesh->PolygonIndex.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitIBData;
	InitIBData.pSysMem = m_Mesh->PolygonIndex.data();
	InitIBData.SysMemPitch = 0;
	InitIBData.SysMemSlicePitch = 0;
	pDevice->CreateBuffer(&bd, &InitIBData, &m_pIndexBuffer);
}

//頂点バッファ作成
void MyMeshData::CreateVertBuffer(ComPtr<ID3D11Device> pDevice)
{
	if (pDevice == NULL)
	{
		return;
	}

	m_pVertexBuffer.Reset();
	int Index;
	MyVertex Vert;
	RefBone	 Bone;
	MyVertex IndexedVertex;
	int PolygonCount;
	PolygonCount = m_Mesh->PolygonCount;

//	for (int j = 0; j < PolygonCount; j++)
//	{
//		Index = m_Mesh->PolygonIndex[j];
//		Vert = m_Mesh->m_pVertex->at(Index);
//		IndexedVertex.Position = Vert.Position;
//		IndexedVertex.Normal = Vert.Normal;
//		IndexedVertex.UV = Vert.UV;
//
//		Bone = m_Mesh->m_pRefBone->at(Index);
//
//		/*switch (m_Mesh->m_pRefBone->size())
//		{
//		case 4:
//			IndexedVertex.BoneId.w = Bone.BoneId[3];
//			IndexedVertex.BoneWeight.w = Bone.BoneId[3];
//		case 3:
//			IndexedVertex.BoneId.w = Bone.BoneId[2];
//			IndexedVertex.BoneWeight.w = Bone.BoneId[2];
//		case 2:
//			IndexedVertex.BoneId.w = Bone.BoneId[1];
//			IndexedVertex.BoneWeight.w = Bone.BoneId[1];
//		case 1:
//			IndexedVertex.BoneId.w = Bone.BoneId[0];
//			IndexedVertex.BoneWeight.w = Bone.BoneId[0];
//		default:
//			break;
//		}
//*/
//		Vertex.push_back(IndexedVertex);
//	}
//
	Vertex = *m_Mesh->m_pVertex.get();

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MyVertex)*Vertex.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitVBData;
	InitVBData.pSysMem = Vertex.data();
	InitVBData.SysMemPitch = 0;
	InitVBData.SysMemSlicePitch = 0;

	HRESULT hr;
	hr=pDevice->CreateBuffer(&bd, &InitVBData, &m_pVertexBuffer);
}

//マテリアルバッファ作成
void MyMeshData::CreateMaterialBuffer(ComPtr<ID3D11Device> pDevice)
{
	if (pDevice == NULL)
	{
		return;
	}

	D3D11_BUFFER_DESC MBDesc;
	MBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MBDesc.ByteWidth = sizeof(MyLambert);
	MBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MBDesc.MiscFlags = 0;
	MBDesc.StructureByteStride = 0;
	MBDesc.Usage = D3D11_USAGE_DYNAMIC;

	pDevice->CreateBuffer(&MBDesc, NULL, &m_pMaterialBuffer);

	//テクスチャー用サンプラー作成
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	pDevice->CreateSamplerState(&SamDesc, m_pSampleLiner.GetAddressOf());

}

void MyMeshData::SetVShader(ComPtr<ID3D11VertexShader> pVShader)
{
	m_pVShader = pVShader;
}

void MyMeshData::SetPShader(ComPtr<ID3D11PixelShader> pPShader)
{
	m_pPShader = pPShader;
}

void MyMeshData::Release()
{
	for (int i = 0; i < m_pTexture.size(); i++)
		m_pTexture[i].Reset();

	m_pSampleLiner.Reset();
}

MyMeshContainer::MyMeshContainer()
{}

MyMeshContainer::MyMeshContainer(vector<MyMeshData> pMeshies)
{
	m_pMeshies = pMeshies;
}

MyMeshContainer::~MyMeshContainer()
{
	Release();
}

MyMeshData MyMeshContainer::GetMesh(int Index)
{
	return m_pMeshies[Index];
}

int MyMeshContainer::GetMeshCount()
{
	return m_pMeshies.size();
}

void MyMeshContainer::Release()
{
	m_pMeshies.clear();
}