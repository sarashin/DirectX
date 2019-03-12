#pragma once
#include"Transform.h"
#include"MySkelton.h"

using namespace fbxsdk;
using namespace Microsoft::WRL;
using namespace std;
//色の構造体
struct Color
{
	float r;
	float g;
	float b;
};
//シェーダに渡す値
struct ShaderGrobal
{
	XMMATRIX WorldMat;
	XMMATRIX ViewMat;
	XMMATRIX ProjMat;
	XMFLOAT4 LightVec;
};

//ランバートマテリアル
struct MyLambert
{
	XMFLOAT4 Ambient;
	XMFLOAT3 Diffuse;
	float	 Alpha;
};

//フォングマテリアル
struct MyPhong : public MyLambert
{
	Color Speculer;
	float Shiness;
	float Refrect;
};
//テクスチャ群
class TextureStocker
{
public:
	map<string, ID3D11ShaderResourceView*> Textures;
};

//頂点
class MyVertex
{
public:
	XMFLOAT4 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 UV;
};

//class VertexStruct
//{
//public:
//	XMFLOAT4 Position;
//	XMFLOAT3 Normal;
//	XMFLOAT2 UV;
//	XMFLOAT4 BoneId;
//	XMFLOAT4 BoneWeight;
//};

//ボーン
class BoneMatData
{
public:
	XMMATRIX BoneMat[4];
};

//ポリゴン
class MyPolygon
{
public:
	vector<int> PolygonIndex;
};
//メッシュ
class MyMesh
{
public:
	MyMesh();
	MyMesh(shared_ptr<vector<MyVertex>> Vertex);
	MyMesh(vector<XMFLOAT4> Positions, vector<XMFLOAT3> Normals, vector<XMFLOAT2> UVs, vector<int> Index);
	~MyMesh();

	int VertexCount;
	int PolygonCount;
	int PolygonVertexCount;

	shared_ptr<vector<MyVertex>>	m_pVertex;
	shared_ptr<vector<RefBone>>		m_pRefBone;

	vector<int>			PolygonIndex;
	int MaterialId;
};

//メッシュオブジェクト マテリアルと1：1の関係で1モデルで複数作られる
class MyMeshData
{
protected:
	shared_ptr<MyMesh>							m_Mesh;		//メッシュ
	vector<ComPtr<ID3D11ShaderResourceView>>	m_pTexture; //テクスチャ
	vector<MyVertex>							Vertex;		//インデックスで並び替えた頂点
	
public:
	MyLambert									m_Material; //マテリアル

	ComPtr<ID3D11VertexShader>		m_pVShader;
	ComPtr<ID3D11PixelShader>		m_pPShader;

	ComPtr<ID3D11Buffer>			m_pVertexBuffer;
	ComPtr<ID3D11Buffer>			m_pIndexBuffer;
	ComPtr<ID3D11Buffer>			m_pMaterialBuffer;
	ComPtr<ID3D11SamplerState>		m_pSampleLiner;

	MyMeshData();
	MyMeshData(MyMesh Mesh,MyLambert Lambert, vector<ComPtr<ID3D11ShaderResourceView>> Tex);
	~MyMeshData();

	int					GetVertCount();
	int					GetIndexCount();
	int					GetPolyIndexCount();
	int					GetTexCount();
	shared_ptr<MyMesh>	GetMesh();
	MyLambert			GetMaterial();
	//vector<VertexStruct>GetVertex();
	XMFLOAT4			GetVertPos(int Index);
	vector<int>			GetIndex();

	ComPtr<ID3D11ShaderResourceView> GetTexture(int Index);

	void SetMesh(shared_ptr<MyMesh> Mesh);
	void SetMaterial(MyLambert Material);
	void SetTexture(vector<ComPtr<ID3D11ShaderResourceView>> SRView);

	void AddTexture(ComPtr<ID3D11ShaderResourceView> SRView);

	void CreateVertBuffer(ComPtr<ID3D11Device> pDevice);
	void CreateIndexBuffer(ComPtr<ID3D11Device> pDevice);
	void CreateMaterialBuffer(ComPtr<ID3D11Device> pDevice);

	void SetVShader(ComPtr<ID3D11VertexShader> pVShader);
	void SetPShader(ComPtr<ID3D11PixelShader> pPShader);

	void Release();
};

class MyMeshContainer
{
	vector<MyMeshData>		m_pMeshies;

public:
	MyMeshContainer();
	MyMeshContainer(vector<MyMeshData> pMeshies);
	~MyMeshContainer();

	MyMeshData GetMesh(int Index);
	int GetMeshCount();
	void Release();
};