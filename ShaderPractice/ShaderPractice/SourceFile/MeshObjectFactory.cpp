#include "stdafx.h"
#include "MeshObjectFactory.h"

using namespace std;
using namespace fbxsdk;

MeshFactory::MeshFactory()
{
	m_pTexture = make_unique<TextureStocker>();
}

MeshFactory::~MeshFactory()
{
	m_pTexture.reset();
}

void MeshFactory::Init(shared_ptr<D3DRenderer> pDevice)
{
	m_pDevice = pDevice;
}

bool MeshFactory::CreateMeshObject(const char* FName,MyMeshContainer& pContainer, MyAnimationContainer& pAnimContainer)
{
	//�ϐ�������
	FbxManager* pFbxManager = FbxManager::Create();
	FbxImporter* pImporter = FbxImporter::Create(pFbxManager, "");
	FbxIOSettings* pIOSettings = FbxIOSettings::Create(pFbxManager, "");
	pFbxManager->SetIOSettings(pIOSettings);

	//�t�@�C�����J��
	int FileFormat = -1;
	if (FileFormat==NULL)
	{
		// ���s
		return false;
	}

	if (!pImporter->Initialize(FName,FileFormat,pFbxManager->GetIOSettings()))
	{
		//�t�@�C���������܂��͑Ή��o�[�W�����łȂ��Ȃ�I��
		return false;
	}

	//�V�[���I�u�W�F�N�g�쐬
	FbxScene* pFbxScene = FbxScene::Create(pFbxManager, FName);
	pImporter->Import(pFbxScene);
	pImporter->Destroy();
	
	FbxGeometryConverter Converter(pFbxManager);
	Converter.Triangulate(pFbxScene,true);
	Converter.RemoveBadPolygonsFromMeshes(pFbxScene);
	//Converter.SplitMeshesPerMaterial(pFbxScene, true);

	///�ǂݍ��݊J�n-----------------------------------------------------

	int MeshNum = pFbxScene->GetSrcObjectCount<FbxMesh>();
	
	vector<MyVertex> VertexArray;
	MyLambert Lambert;
	vector<ComPtr<ID3D11ShaderResourceView>> pTex;
	shared_ptr<MyMesh> pMyMesh;
	vector<MyMesh>		Mesh;
	vector<RefBone>		Bone;
	vector<MyMeshData> pMeshData;
	
	shared_ptr<MySkelton> pSkelton;

	ID3D11Device* pDevice = m_pDevice->GetDevice();

	vector<MySkinAnimation> Animation;

	for (int i = 0; i < MeshNum; i++)
	{
		FbxMesh* pMesh = pFbxScene->GetSrcObject<FbxMesh>(i);
		//���_���̓C���f�b�N�X���Q�Ƃ��ĕ������1�������
		CreatePolyGroup(pMesh, Mesh);
		//CreateMesh(pMesh, VertexArray);

		int MaterialNum = pMesh->GetNode()->GetSrcObjectCount<FbxSurfaceMaterial>();
		pMeshData.resize(MaterialNum);
		Bone.resize(Mesh[0].VertexCount);
		CreateAnimInfo(pMesh, Bone, Animation);

		//�}�e���A���ƃ��b�V���̍쐬
		for (int j = 0; j < MaterialNum; j++)
		{
			CreateMaterial(pMesh, Lambert, pTex, j);
			Mesh[j].m_pRefBone = make_shared<vector<RefBone>>(Bone);
			pMeshData[j] = MyMeshData(Mesh[j], Lambert, pTex);

			//�f�t�H���g�V�F�[�_��ݒ�
			pMeshData[j].SetVShader(m_pDevice->m_pDefVShader);
			pMeshData[j].SetPShader(m_pDevice->m_pDefPShader);


			pMeshData[j].CreateVertBuffer(pDevice);
			pMeshData[j].CreateIndexBuffer(pDevice);
			pMeshData[j].CreateMaterialBuffer(pDevice);
		}

	}

	pContainer = MyMeshContainer(pMeshData);
	pAnimContainer = MyAnimationContainer(Animation);
	///�ǂݍ��ݏI��----------------------------------------------------
	
	pMyMesh.reset();
	pTex.clear();
	pSkelton.reset();
	pFbxManager->Destroy();

	return true;
}

//XMMATRIX�֌^�ϊ�
void CastMatrix(XMMATRIX* Mat, FbxMatrix FbxMat)
{
	Mat->m[0][0] = FbxMat.mData[0][0];
	Mat->m[0][1] = FbxMat.mData[0][1];
	Mat->m[0][2] = FbxMat.mData[0][2];
	Mat->m[0][3] = FbxMat.mData[0][3];
	Mat->m[1][0] = FbxMat.mData[1][0];
	Mat->m[1][1] = FbxMat.mData[1][1];
	Mat->m[1][2] = FbxMat.mData[1][2];
	Mat->m[1][3] = FbxMat.mData[1][3];
	Mat->m[2][1] = FbxMat.mData[2][0];
	Mat->m[2][2] = FbxMat.mData[2][1];
	Mat->m[2][3] = FbxMat.mData[2][2];
	Mat->m[2][1] = FbxMat.mData[2][3];
	Mat->m[3][0] = FbxMat.mData[3][0];
	Mat->m[3][1] = FbxMat.mData[3][1];
	Mat->m[3][2] = FbxMat.mData[3][2];
	Mat->m[3][3] = FbxMat.mData[3][3];
}

//�m�[�h�T�����ăQ�[���I�u�W�F�N�g�ɕK�v�ȃf�[�^�𓾂�
//void MeshFactory::RecursiveNode(FbxNode* pNode, shared_ptr<MyMeshContainer> pMeshContainer)
//{
//	if (pNode==NULL)
//		return;
//
//	MyAnimationContainer DummyContainer;
//	FbxMesh* pMesh=NULL;
//	FbxNodeAttribute* pAttribute = pNode->GetNodeAttribute();
//
//	if (pAttribute == NULL)
//		return;
//
//	FbxNodeAttribute::EType eType = pAttribute->GetAttributeType();
//	MyLambert Lambert;
//	ComPtr<ID3D11ShaderResourceView> Tex;
//	int MatNum, TexNum;
//
//	switch (eType)
//	{
//	case fbxsdk::FbxNodeAttribute::eUnknown:
//	case fbxsdk::FbxNodeAttribute::eNull:
//		break;
//	case fbxsdk::FbxNodeAttribute::eMesh:	//���b�V����������
//		pMesh = (FbxMesh*)pAttribute;
//		pMyMeshData.SetMesh(CreateMesh(pMesh));
//		CreateMaterial(pMesh, Lambert, MatNum, Tex, TexNum);
//		pMyMeshData.SetMaterial(Lambert);
//		pMyMeshData.SetTexture(Tex);
//		//GetAnimInfo(pMesh, &DummyContainer);
//		break;
//	default:
//		break;
//	}
//	
//
//	//�ċA����
//	int lChildNodeNum = pNode->GetChildCount();
//	FbxNode* ChildNode;
//
//	for (int i = 0; i < lChildNodeNum; i++)
//	{
//		ChildNode = pNode->GetChild(i);
//		RecursiveNode(ChildNode, pMyMeshData);
//	}
//}

//unique_ptr<MyMesh> MeshObjectFactory::CreateObject(FbxMesh* pMesh)
//{
//	MyMesh Mesh;
//	MyLambert* Lambert=new MyLambert;
//	ID3D11ShaderResourceView** Tex;
//	int MatNum, TexNum;
//	CreateMesh(pMesh, &Mesh);
//	CreateMaterial(pMesh, &Lambert, MatNum, &Tex, TexNum);
//	//make_unique<MyMesh>(Mesh, Lambert, MatNum, Tex, TexNum);
//	return make_unique<MyMesh>(Mesh);
//}

//���_���擾
void SetVert(FbxMesh* pMesh, vector<XMFLOAT4>& pVertex)
{
	int VertexCount = pMesh->GetControlPointsCount();	//���_��
	FbxVector4* Vertices = pMesh->GetControlPoints();			//���_�C���f�b�N�X�z��
	vector<XMFLOAT4> Vertex;				//���_���
	int i;
	Vertex.resize(VertexCount);

	for (i = 0; i < VertexCount; ++i)
	{
		//���_���W�擾 
		Vertex[i].x = (FLOAT)Vertices[i][0];
		Vertex[i].y = (FLOAT)Vertices[i][1];
		Vertex[i].z = (FLOAT)Vertices[i][2];
		Vertex[i].w = (FLOAT)Vertices[i][3];
	}

	pVertex = Vertex;
}

//�|���S���̃C���f�b�N�X�擾
void SetIndex(FbxMesh* pMesh, vector<int>& pIndex)
{
	if (pMesh == NULL)
	{
		return;
	}

	int PolygonNum = pMesh->GetPolygonCount();					//�|���S����
	int PolygonVertexNum = pMesh->GetPolygonVertexCount();		//�|���S���̒��_�C���f�b�N�X��
	int* Vertics = pMesh->GetPolygonVertices();					//�|���S���̒��_�C���f�b�N�X�z��
	vector<int> Poly;
	Poly.resize(PolygonNum * 3);
	
	//MeshData->m_pMesh->Index.resize(PolygonNum);
	int LayerCount = pMesh->GetLayerCount();

	int PolyInMaterial = pMesh->GetPolygonCount();
	int PolyNum=0;
	
	for (int i = 0; i < PolyInMaterial;i++)
	{
		int IndexNumInPoly = pMesh->GetPolygonSize(i);			//i�Ԗڂ̃|���S���̒��_��

		for (int j = 0; j < IndexNumInPoly; j++)
		{
			Poly[PolyNum + j] = pMesh->GetPolygonVertex(i, j);	//i�Ԗڂ̃|���S����j�Ԗڂ̒��_�̃C���f�b�N�X�ԍ�
		}

		PolyNum += IndexNumInPoly;
	}
	
	pIndex = Poly;
}

//�@���擾
void SetNormal(FbxLayer* Layer ,vector<XMFLOAT3>& pNormal)
{
	//�@���f�[�^�̎擾
	vector<XMFLOAT3> Normal;

	FbxLayerElementNormal* NormalElem = Layer->GetNormals();
	if (NormalElem == NULL)
	{
		return;   // �@������
	}

	int    NormalNum = NormalElem->GetDirectArray().GetCount();
	int    IndexNum = NormalElem->GetIndexArray().GetCount();
	Normal.resize(NormalNum);

	//�}�b�s���O���[�h
	FbxLayerElement::EMappingMode   NormalMapMode = NormalElem->GetMappingMode();
	//�擾���@
	FbxLayerElement::EReferenceMode NormalRefMode = NormalElem->GetReferenceMode();

	if (NormalMapMode == FbxLayerElement::eByPolygonVertex)
	{
		if (NormalRefMode == FbxLayerElement::eDirect)
		{
			// ���ڎ擾
			for (int i = 0; i < NormalNum; ++i) {
				Normal[i].x = (float)NormalElem->GetDirectArray().GetAt(i)[0];
				Normal[i].y = (float)NormalElem->GetDirectArray().GetAt(i)[1];
				Normal[i].z = (float)NormalElem->GetDirectArray().GetAt(i)[2];
			}
		}
	}
	else if (NormalMapMode == FbxLayerElement::eByControlPoint)
	{
		if (NormalRefMode == FbxLayerElement::eDirect)
		{
			// ���ڎ擾
			for (int i = 0; i < NormalNum; ++i) {
				Normal[i].x = (float)NormalElem->GetDirectArray().GetAt(i)[0];
				Normal[i].y = (float)NormalElem->GetDirectArray().GetAt(i)[1];
				Normal[i].z = (float)NormalElem->GetDirectArray().GetAt(i)[2];
			}
		}
	}

	pNormal = Normal;
}

void SetUV(FbxLayer* pLayer, vector<XMFLOAT2>& pUV)
{
	//UV�擾
	FbxLayerElementUV* UVElem = pLayer->GetUVs();

	int UVCount = UVElem->GetDirectArray().GetCount();	//UV���W�̐�
	int UVindexCount = UVElem->GetIndexArray().GetCount();//UV�C���f�b�N�X�̐�
	int Size = UVCount > UVindexCount ? UVCount : UVindexCount;	//���̑傫�������T�C�Y��
	vector<XMFLOAT2> UV;
	UV.resize(Size);
	vector<int> s;
	
	//�}�b�s���O���[�h
	FbxLayerElement::EMappingMode UVMapMode = UVElem->GetMappingMode();
	//�擾���@
	FbxLayerElement::EReferenceMode RefMode = UVElem->GetReferenceMode();

	if (UVMapMode == FbxLayerElement::eByPolygonVertex)
	{
		FbxArray<FbxVector2> UVArray;
		UVElem->GetDirectArray().CopyTo(UVArray);

		if (RefMode == FbxLayerElement::eDirect)
		{
			// ���ڎ擾
			for (int j = 0; j < Size; ++j) {
				UV[j].x = (float)UVArray.GetAt(j)[0];
				UV[j].y = (float)UVArray.GetAt(j)[1];
			}
		}
		else if (RefMode == FbxLayerElement::eIndexToDirect)
		{
			// �C���f�b�N�X����擾
			int index;

			for (int j = 0; j <Size; ++j)
			{
				index = UVElem->GetIndexArray().GetAt(j);
				s.push_back(index);
				UV[j].x = (float)UVArray.GetAt(index)[0];
				UV[j].y = 1.0-(float)UVArray.GetAt(index)[1];
			}
		}
	}

	pUV = UV;
	UVElem = NULL;
}

//���b�V���̎擾
void MeshFactory::CreateMesh(FbxMesh* pMesh, shared_ptr<vector<MyVertex>> pVertex)
{
	int PolygonNum = pMesh->GetPolygonCount();			//�|���S����
	int VertexCount = pMesh->GetControlPointsCount();	//���_��
	if (!(PolygonNum > 1))
		return;
	
	vector<XMFLOAT4>	Vertex;		//���_���W
	vector<int>			Poly;		//�|���S���C���f�b�N�X
	vector<XMFLOAT3>	Normal;		//�@���x�N�g��
	vector<XMFLOAT2>	UV;			//UV���W

	int LayerNum = pMesh->GetLayerCount();
	const char* Name=pMesh->GetName();
	FbxLayer* Layer = pMesh->GetLayer(0);
	//FbxLayerElementArrayTemplate<int>* PolygonMaterial;

	//PolygonMaterial = &pMesh->GetLayer(0)->GetMaterials()->GetIndexArray();
	///*if (!pMesh->GetMaterialIndices(&PolygonMaterial))
	//	return;*/

	//for (int i = 0; i < PolygonMaterial->GetCount(); i++)
	//	Poly.push_back(PolygonMaterial->GetAt(i));

	Poly.resize(PolygonNum * 3);
	Normal.resize(PolygonNum * 3);
	SetVert(pMesh, Vertex);
	SetIndex(pMesh, Poly);

	//UV�Ɩ@���𒸓_�ʒu�̃C���f�b�N�X�Ń\�[�g
	int PolygonVertexNum = Poly.size();
	//�@���擾
	SetNormal(Layer, Normal);

	vector<XMFLOAT3> SortNorm;
	SortNorm.resize(VertexCount);

	for (int i = 0; i < PolygonNum; i++)
		SortNorm[Poly[i]] = Normal[i];

	//UV���W�擾
	SetUV(Layer, UV);

	vector<XMFLOAT2> SortUV;
	SortUV.resize(VertexCount);

	for (int i = 0; i < PolygonNum; i++)
		SortUV[Poly[i]] = UV[i];

	//�f�[�^�i�[
	MyMesh Mesh;
	pVertex->resize(VertexCount);
	
	for (int i = 0; i < VertexCount; i++)
	{
		pVertex->at(i).Position = Vertex[i];
		pVertex->at(i).Normal	= SortNorm[i];
		pVertex->at(i).UV		= SortUV[i];
	}

	Vertex.clear();
	Poly.clear();
	Normal.clear();
	UV.clear();
}

void MeshFactory::CreatePolyGroup(FbxMesh* pMesh, vector<MyMesh>& Opt)
{
	int MaterialNum = pMesh->GetNode()->GetMaterialCount();
	FbxLayerElementArrayTemplate<int>* MaterialIndex;
	pMesh->GetMaterialIndices(&MaterialIndex);
	shared_ptr<vector<MyVertex>> Vertex=make_shared<vector<MyVertex>>();
	CreateMesh(pMesh, Vertex);
	Opt.resize(MaterialNum);
	int PolygonNum = MaterialIndex->GetCount();
	int* Vertics = pMesh->GetPolygonVertices();					//�|���S���̒��_�C���f�b�N�X�z��

	for (int i = 0; i < PolygonNum; i++)
	{
		int PolygonVertexNum = pMesh->GetPolygonSize(i);

		for(int j=0;j<PolygonVertexNum;j++)
			Opt[MaterialIndex->GetAt(i)].PolygonIndex.push_back(Vertics[i]);

	}

	for (int i = 0; i < Opt.size(); i++)
	{
		Opt[i].m_pVertex = Vertex;
		Opt[i].VertexCount = Opt[i].m_pVertex->size();
		Opt[i].PolygonCount = Opt[i].PolygonIndex.size();
		Opt[i].PolygonVertexCount = Opt[i].PolygonCount * 3;
	}
}

void SetLambertInfo(FbxSurfaceLambert* Lambert, MyLambert* Material)
{
	//�A���r�G���g�J���[
	Material->Ambient.x = Lambert->Ambient.Get()[0];
	Material->Ambient.y = Lambert->Ambient.Get()[1];
	Material->Ambient.z = Lambert->Ambient.Get()[2];
	Material->Ambient.w = 1.0;

	//�f�B�t���[�Y
	Material->Diffuse.x = Lambert->Diffuse.Get()[0];
	Material->Diffuse.y = Lambert->Diffuse.Get()[1];
	Material->Diffuse.z = Lambert->Diffuse.Get()[2];
	

	////�G�~�b�V�u
	//Material->Emissive.x = Lambert->Emissive.Get()[0];
	//Material->Emissive.y = Lambert->Emissive.Get()[1];
	//Material->Emissive.z = Lambert->Emissive.Get()[2];

	////�o���v
	//Material->Bump.x = Lambert->Bump.Get()[0];
	//Material->Bump.y = Lambert->Bump.Get()[1];
	//Material->Bump.z = Lambert->Bump.Get()[2];

	//����
	Material->Alpha = 1.0f;
}

void SetPhongInfo(FbxSurfacePhong* Phong, MyPhong* Material)
{
	//�����o�[�g�Ɠ��������擾
	SetLambertInfo(Phong, Material);

	//�X�y�L����
	Material->Speculer.r = Phong->Specular.Get()[0];
	Material->Speculer.g = Phong->Specular.Get()[1];
	Material->Speculer.b = Phong->Specular.Get()[2];
	//����
	Material->Shiness = Phong->Shininess.Get();
	//���ˌ�
	Material->Refrect = Phong->ReflectionFactor.Get();
}

void SetMaterial()
{
}

void MeshFactory::CreateMaterial(FbxMesh* pMesh, MyLambert& MaterialData, vector<ComPtr<ID3D11ShaderResourceView>>& pTex,int MeshNum)
{
	FbxNode* Node = pMesh->GetNode();			//���b�V���̃m�[�h�擾

	if (Node == NULL)
	{
		return;
	}

	int MaterialNum = Node->GetMaterialCount();	//�}�e���A����
	if (MaterialNum == 0)
	{
		return;
	}
	//�}�e���A���i�[
	MyLambert	DummyMaterial;

	FbxSurfaceMaterial* Material = Node->GetMaterial(MeshNum);
	
	if (Material == NULL)
	{
		return;
	}

	if (Material->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		//�����o�[�g�}�e���A��
		FbxSurfaceLambert* Lambert = (FbxSurfaceLambert*)Material;
		SetLambertInfo(Lambert, &MaterialData);
	}
	else if (Material->GetClassId().Is(FbxSurfacePhong::ClassId))
	{
		//�t�H���O�}�e���A��
		FbxSurfacePhong* Phong = (FbxSurfacePhong*)Material;
	}
	else
	{
		//���̑�
		SetLambertInfo(NULL, &MaterialData);
	}

	FbxProperty DiffuseProperty = Material->FindProperty(FbxSurfaceMaterial::sDiffuse);
	int layerNum = DiffuseProperty.GetSrcObjectCount(FbxCriteria::ObjectType(FbxLayeredTexture::ClassId));

	//�e�N�X�`���擾
	if (layerNum > 0)
	{
		//���C���[�h�e�N�X�`��
	}
	else
	{
		//�e�N�X�`�����
		int TexNum = DiffuseProperty.GetSrcObjectCount(FbxCriteria::ObjectType(FbxTexture::ClassId));
		if (TexNum == 0)
			return;

		//pTex.resize(TexNum);
			
		FbxFileTexture* FileTex = FbxCast<FbxFileTexture>(DiffuseProperty.GetSrcObject(FbxCriteria::ObjectType(FbxTexture::ClassId),0));
		if (FileTex == NULL)
			return;
		FbxString Path = FbxPathUtils::GetFileName(FileTex->GetFileName());
		const char* TexPath = Path;

		//���΃p�X�֕ϊ�
		char RelativePath[255] = "../ShaderPractice/Resource/";
		strcat_s(RelativePath, 255, TexPath);

		//�p�X����e�N�X�`�����擾
		HRESULT hr;
		ID3D11ShaderResourceView* Tex;
		hr = D3DX11CreateShaderResourceViewFromFileA(m_pDevice->GetDevice(), RelativePath, NULL, NULL, &Tex, NULL);

		if (FAILED(hr))
		{
			TexNum = 0;
			return;
		}

		m_pTexture->Textures[TexPath] = Tex;
		pTex.push_back(Tex);

		/*auto It = m_pTexture->Textures.find(TexPath);

		if (It == m_pTexture->Textures.end())
		{
			m_pTexture->Textures[TexPath] = Tex;
		}
		else
		{
			pTex.push_back(It->second);
		}*/

	}
	
}

//�A�j���[�V�����擾
void MeshFactory::CreateAnimInfo(FbxMesh* pMesh,vector<RefBone>& pBone, vector<MySkinAnimation>& pAnim)
{
	FbxNode* pNode = pMesh->GetNode();		//�m�[�h
	FbxScene* AnimScene = pMesh->GetScene();//�V�[��

	FbxTime::EMode TimeMode;				//�^�C�����[�h�擾
	TimeMode = AnimScene->GetGlobalSettings().GetTimeMode();

	FbxTime Period;							//1�t���[���̎���
	Period.SetTime(0, 0, 0, 1, 0, TimeMode);

	FbxArray<FbxString*> NameArray;			//�A�j���[�V������
	AnimScene->FillAnimStackNameArray(NameArray);
	int AnimNum = NameArray.GetCount();		//�A�j���[�V������

	FbxTakeInfo* TakeInfo = new FbxTakeInfo;//�e�C�N�̏��
	FbxTime Start;							//�A�j���[�V�����X�^�[�g����
	FbxTime Stop;							//�I������
	int startFrame;							//�A�j���[�V�����X�^�[�g�t���[��
	int stopFrame;							//�I���t���[��
	FbxMatrix MatrixData;					//�p��
	vector<MySkinAnimation> Animation;
	Animation.resize(AnimNum);

	for (int i = 0; i < AnimNum; i++)
	{
		TakeInfo = AnimScene->GetTakeInfo(*(NameArray[i]));
		if (TakeInfo)
		{
			//continue;
		}

		//1�e�C�N�̊J�n�A�I�����Ԏ擾
		Start = TakeInfo->mLocalTimeSpan.GetStart();
		Stop  = TakeInfo->mLocalTimeSpan.GetStop();
		Animation[i].Start = (int)(Start.Get() / Period.Get());
		Animation[i].Stop = (int)(Stop.Get() / Period.Get());

		Animation[i].m_AnimMatrix.resize(Animation[i].Stop - Animation[i].Start);
		//��{�[���̎p���擾
		for (int j = 0; j < Animation[i].Stop-Animation[i].Start; j++)
		{
			MatrixData = pNode->EvaluateGlobalTransform(Period*j);
			CastMatrix(&Animation[i].m_AnimMatrix[j], MatrixData);
		}
		//�{�[���̏��擾
		CreateBoneInfo(pMesh, pBone,Animation[i], Animation[i].Start, Animation[i].Stop);

		////�A�j���[�V�����R���g���[���ɃZ�b�g
		string Name = NameArray[i]->Buffer();
		Animation[i].Name = Name;
		pAnim.push_back(Animation[i]);
	}


	/*D3D11_BUFFER_DESC BBDesc;
	BBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BBDesc.ByteWidth = sizeof(XMMATRIX)*Animation->m_Skelton->GetBoneNum();
	BBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BBDesc.MiscFlags = 0;
	BBDesc.StructureByteStride = 0;
	BBDesc.Usage = D3D11_USAGE_DYNAMIC;

	m_pDevice->CreateBuffer(&BBDesc, NULL, &AnimContainer->m_BoneBuffer);*/

	TakeInfo = NULL;
	NameArray = NULL;
}

void SetBone(FbxCluster* pCluster,vector<RefBone>& pBone, int ClusterNum)
{
	RefBone Bone;			//�{�[��
	int CPCount = pCluster->GetControlPointIndicesCount();

	if (CPCount == 0)
		return;

	int* Vertices = pCluster->GetControlPointIndices();
	double* Weights =pCluster->GetControlPointWeights();

	for (int i = 0; i < CPCount; i++)
	{
		/*pBone.BoneId[Vertices[i]].push_back(ClusterNum);
		pBone.Weight[Vertices[i]].push_back((float)Weights[i]);
*/
		pBone[Vertices[i]].BoneId.push_back(ClusterNum);
		pBone[Vertices[i]].Weight.push_back((float)Weights[i]);
	}

	Vertices = NULL;
	Weights = NULL;
}

void SetAnim(FbxCluster* pCluster, MySkinAnimation& pAnim, int ClusterNum, int StartFrame, int StopFrame)
{
	int CPCount = pCluster->GetControlPointIndicesCount();

	if (CPCount == 0)
		return;

	shared_ptr<MySkelton> pSkelton = pAnim.m_Skelton;

	FbxAMatrix  InitMat;	//�����p��						
	FbxMatrix	FrameMat;	//�t���[���p��
	XMMATRIX	CastMat;	//�ϊ��p�p��

	pCluster->GetTransformLinkMatrix(InitMat);
	CastMatrix(&pSkelton->m_Bone[ClusterNum].m_InitMat, InitMat);
	XMMatrixInverse(pSkelton->m_Bone[ClusterNum].m_InitMat.r, pSkelton->m_Bone[ClusterNum].m_InitMat);
	pAnim.m_AnimBoneMatrix[ClusterNum].reserve(StopFrame - StartFrame);

	//�e�t���[���̎p���擾
	for (int i = 0; i < StopFrame-StartFrame; i++)
	{
		FrameMat = pCluster->GetLink()->EvaluateGlobalTransform(i);
		CastMatrix(&CastMat, FrameMat);
		pAnim.m_AnimBoneMatrix[ClusterNum].push_back(CastMat);
	}
}

//�{�[���̏��擾
void MeshFactory::CreateBoneInfo(FbxMesh* pMesh, vector<RefBone>& pBone,MySkinAnimation& pAnim, int StartFrame,int StopFrame)
{
	int SkinCount= pMesh->GetDeformerCount(FbxDeformer::eSkin);	//�X�L����
	FbxSkin* pSkin;												//�X�L��
	int ClusterNum;											//�N���X�^��
	FbxCluster* pCluster;										//�N���X�^
	
	for (int i = 0; i < SkinCount; i++)
	{
		//�X�L�����擾
		pSkin = (FbxSkin*)pMesh->GetDeformer(i, FbxDeformer::eSkin);
		ClusterNum = pSkin->GetClusterCount();
		//�{�[���̎擾
		for (int j = 0; j < ClusterNum; j++)
		{
			pCluster = pSkin->GetCluster(j);
			MyBone Bone;
			SetBone(pCluster, pBone, j);
			pAnim.m_Skelton->m_Bone.resize(ClusterNum);
			pAnim.m_AnimBoneMatrix.resize(ClusterNum);
			SetAnim(pCluster, pAnim, j, StartFrame, StopFrame);
		}
	}

	pSkin = NULL;
	pCluster = NULL;
}

//�X�v���C�g�쐬
bool MeshFactory::CreateSprite(const char* FileName, shared_ptr<Sprite>& pSprite, RECT Rect, RECT Size)
{
	MyMesh Mesh;
	//�X�v���C�g�̒��_��`
	int VertexCount = 6;
	vector<MyVertex> Vert;
	Vert.resize(VertexCount);
	Vert[0].Position = XMFLOAT4(Size.left, Size.top, 0, 0);
	Vert[0].Normal = XMFLOAT3(0, -1, 0);
	Vert[0].UV= XMFLOAT2(Rect.left, Rect.top);
	Vert[1].Position = XMFLOAT4(Size.right, Size.top, 0, 0);
	Vert[1].Normal = XMFLOAT3(0, -1, 0);
	Vert[1].UV = XMFLOAT2(Rect.right, Rect.top);
	Vert[2].Position = XMFLOAT4(Size.left, Size.bottom, 0, 0);
	Vert[2].Normal = XMFLOAT3(0, -1, 0);
	Vert[2].UV = XMFLOAT2(Size.left, Size.bottom);
	Vert[3].Position = XMFLOAT4(Size.right, Size.top, 0, 0);
	Vert[3].Normal = XMFLOAT3(0, -1, 0);
	Vert[3].UV = XMFLOAT2(Size.right, Size.top);
	Vert[4].Position = XMFLOAT4(Size.right, Size.bottom, 0, 0);
	Vert[4].Normal = XMFLOAT3(0, -1, 0);
	Vert[4].UV = XMFLOAT2(Size.right, Size.bottom);
	Vert[5].Position = XMFLOAT4(Size.left, Size.bottom, 0, 0);
	Vert[5].Normal = XMFLOAT3(0, -1, 0);
	Vert[5].UV = XMFLOAT2(Size.left, Size.bottom);

	//�X�v���C�g�̒��_�C���f�b�N�X��`
	int PolygonCount = 6;
	vector<int> Index;
	Index.resize(6);
	Index[0] = 0;
	Index[1] = 1;
	Index[2] = 2;
	Index[3] = 3;
	Index[4] = 4;
	Index[5] = 5;
	//Mesh = MyMesh(&Vert[0], &Index[0], VertexCount, PolygonCount);

	//�e�N�X�`��
	ID3D11ShaderResourceView* Tex;

	D3DX11CreateShaderResourceViewFromFile(m_pDevice->GetDevice(), FileName, NULL, NULL, &Tex, NULL);
	//m_pTexture->Textures[FileName] = Tex;
	pSprite = make_shared<Sprite>(Mesh, Tex);
	pSprite->m_Material.Alpha = 1.0;

	//�o�b�t�@�쐬
	pSprite->CreateVertBuffer(m_pDevice->GetDevice());
	pSprite->CreateIndexBuffer(m_pDevice->GetDevice());
	pSprite->CreateMaterialBuffer(m_pDevice->GetDevice());

	//�e�N�X�`���T���v��
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	m_pDevice->GetDevice()->CreateSamplerState(&SamDesc, &pSprite->m_pSampleLiner);

	return true;
}

void MeshFactory::Release()
{
	if (m_pTexture != NULL)
	{
		m_pTexture.reset();
	}
}