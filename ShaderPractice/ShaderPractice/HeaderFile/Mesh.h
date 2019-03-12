#pragma once
#include"Transform.h"
#include"MySkelton.h"

using namespace fbxsdk;
using namespace Microsoft::WRL;
using namespace std;
//�F�̍\����
struct Color
{
	float r;
	float g;
	float b;
};
//�V�F�[�_�ɓn���l
struct ShaderGrobal
{
	XMMATRIX WorldMat;
	XMMATRIX ViewMat;
	XMMATRIX ProjMat;
	XMFLOAT4 LightVec;
};

//�����o�[�g�}�e���A��
struct MyLambert
{
	XMFLOAT4 Ambient;
	XMFLOAT3 Diffuse;
	float	 Alpha;
};

//�t�H���O�}�e���A��
struct MyPhong : public MyLambert
{
	Color Speculer;
	float Shiness;
	float Refrect;
};
//�e�N�X�`���Q
class TextureStocker
{
public:
	map<string, ID3D11ShaderResourceView*> Textures;
};

//���_
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

//�{�[��
class BoneMatData
{
public:
	XMMATRIX BoneMat[4];
};

//�|���S��
class MyPolygon
{
public:
	vector<int> PolygonIndex;
};
//���b�V��
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

//���b�V���I�u�W�F�N�g �}�e���A����1�F1�̊֌W��1���f���ŕ��������
class MyMeshData
{
protected:
	shared_ptr<MyMesh>							m_Mesh;		//���b�V��
	vector<ComPtr<ID3D11ShaderResourceView>>	m_pTexture; //�e�N�X�`��
	vector<MyVertex>							Vertex;		//�C���f�b�N�X�ŕ��ёւ������_
	
public:
	MyLambert									m_Material; //�}�e���A��

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