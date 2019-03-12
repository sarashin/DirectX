#pragma once
#include "GameObject.h"
#include "Sprite.h"
#include "D3Device.h"
#include "Mesh.h"
#include "MyAnimation.h"


class MeshFactory :public Singleton<MeshFactory>
{
	//テクスチャを保持
	shared_ptr<TextureStocker>	m_pTexture;
	shared_ptr<D3DRenderer>		m_pDevice;

	//FBX
	void RecursiveNode(FbxNode* pNode, shared_ptr<MyMeshContainer> pMeshContainer);
	void CreateMesh(FbxMesh* pMesh, shared_ptr<vector<MyVertex>> pVertex);
	void CreatePolyGroup(FbxMesh* pMesh, vector<MyMesh>& pMyMesh);
	void CreateMaterial(FbxMesh* pMesh, MyLambert& MeshData, vector<ComPtr<ID3D11ShaderResourceView>>& pTex, int MeshNum);
	void CreateAnimInfo(FbxMesh* pMesh, vector<RefBone>& pBone, vector<MySkinAnimation>& MeshData);
	void CreateBoneInfo(FbxMesh* pMesh, vector<RefBone>& pBone, MySkinAnimation& MeshData, int StartFrame,int StopFrame);
public:
	MeshFactory();
	~MeshFactory();

	//メッシュオブジェクト作成
	void Init(shared_ptr<D3DRenderer> pDevice);
	bool CreateMeshObject(const char* FName,MyMeshContainer& pMeshContainer,MyAnimationContainer& pAnimContainer);
	bool CreateSprite(const char* FName, shared_ptr<Sprite>& OptSprite,RECT Rect,RECT Size);
	void Release();
};

