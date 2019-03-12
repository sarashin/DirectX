#pragma once

#include "GameObject.h"
#include "Mesh.h"
#include "D3Device.h"
#include "Camera.h"
#include "Light.h"

class MeshObject :public GameObject
{
protected:
	//���b�V��
	MyMeshContainer	m_Mesh;	
public:
	//�\�����Ă�����
	bool			IsAppear;		

	//�o�b�t�@�[
	ComPtr<ID3D11Buffer> m_pMatBuffer;
	
	MeshObject();
	MeshObject(Transform  Trans);
	MeshObject(MyMeshContainer pMesh);
	MeshObject(MyMeshContainer pMesh,Transform Trans);
	~MeshObject();

	virtual void Create(const char* ModelPath, ComPtr<ID3D11Device> pDevice);

	virtual void Update();
	virtual void Draw(Camera Cam,vector<Light> ProjctLight,ComPtr<ID3D11DeviceContext> pRenderer);
	void CopyObject(shared_ptr<MeshObject>& ppObject);
	void Release();
};

