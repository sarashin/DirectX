#pragma once
#include"Mesh.h"

class Sprite
{
protected:
	//板ポリ
	shared_ptr<MyMesh> m_Mesh;
	//テクスチャリソース
	ComPtr<ID3D11ShaderResourceView> m_pTexture;
public:
	//マテリアル
	MyLambert m_Material;
	Sprite();
	Sprite(const Sprite& sprite);
	Sprite(MyMesh Mesh,ID3D11ShaderResourceView* pTexture);
	~Sprite();

	//シェーダへ渡すバッファ
	ComPtr<ID3D11Buffer>		m_pVertexBuffer;
	ComPtr<ID3D11Buffer>		m_pIndexBuffer;
	ComPtr<ID3D11Buffer>		m_pMaterialBuffer;
	ComPtr<ID3D11SamplerState>  m_pSampleLiner;

	shared_ptr<ID3D11VertexShader> m_pVShader;
	shared_ptr<ID3D11PixelShader>  m_pPShader;

	Transform m_LocalMat;

	int							GetVertCount();
	int							GetIndexCount(int i);
	MyMesh						GetMesh();
	MyLambert					GetMaterial();
	MyVertex					GetVertex();

	ID3D11ShaderResourceView**	GetTexture();

	void SetMesh(MyMesh Mesh);
	void SetTexture(ID3D11ShaderResourceView* SRView);
	void SetMaterial(MyLambert Material);

	void CreateVertBuffer(ID3D11Device* Device);
	void CreateIndexBuffer(ID3D11Device* Device);
	void CreateMaterialBuffer(ID3D11Device* Device);

	void Release();
	void Draw(ID3D11DeviceContext* DevContext);
};

