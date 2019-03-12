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
	//変数初期化
	FbxManager* pFbxManager = FbxManager::Create();
	FbxImporter* pImporter = FbxImporter::Create(pFbxManager, "");
	FbxIOSettings* pIOSettings = FbxIOSettings::Create(pFbxManager, "");
	pFbxManager->SetIOSettings(pIOSettings);

	//ファイルを開く
	int FileFormat = -1;
	if (FileFormat==NULL)
	{
		// 失敗
		return false;
	}

	if (!pImporter->Initialize(FName,FileFormat,pFbxManager->GetIOSettings()))
	{
		//ファイルが無いまたは対応バージョンでないなら終了
		return false;
	}

	//シーンオブジェクト作成
	FbxScene* pFbxScene = FbxScene::Create(pFbxManager, FName);
	pImporter->Import(pFbxScene);
	pImporter->Destroy();
	
	FbxGeometryConverter Converter(pFbxManager);
	Converter.Triangulate(pFbxScene,true);
	Converter.RemoveBadPolygonsFromMeshes(pFbxScene);
	//Converter.SplitMeshesPerMaterial(pFbxScene, true);

	///読み込み開始-----------------------------------------------------

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
		//頂点情報はインデックスを参照して分ける為1つだけ作る
		CreatePolyGroup(pMesh, Mesh);
		//CreateMesh(pMesh, VertexArray);

		int MaterialNum = pMesh->GetNode()->GetSrcObjectCount<FbxSurfaceMaterial>();
		pMeshData.resize(MaterialNum);
		Bone.resize(Mesh[0].VertexCount);
		CreateAnimInfo(pMesh, Bone, Animation);

		//マテリアルとメッシュの作成
		for (int j = 0; j < MaterialNum; j++)
		{
			CreateMaterial(pMesh, Lambert, pTex, j);
			Mesh[j].m_pRefBone = make_shared<vector<RefBone>>(Bone);
			pMeshData[j] = MyMeshData(Mesh[j], Lambert, pTex);

			//デフォルトシェーダを設定
			pMeshData[j].SetVShader(m_pDevice->m_pDefVShader);
			pMeshData[j].SetPShader(m_pDevice->m_pDefPShader);


			pMeshData[j].CreateVertBuffer(pDevice);
			pMeshData[j].CreateIndexBuffer(pDevice);
			pMeshData[j].CreateMaterialBuffer(pDevice);
		}

	}

	pContainer = MyMeshContainer(pMeshData);
	pAnimContainer = MyAnimationContainer(Animation);
	///読み込み終了----------------------------------------------------
	
	pMyMesh.reset();
	pTex.clear();
	pSkelton.reset();
	pFbxManager->Destroy();

	return true;
}

//XMMATRIXへ型変換
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

//ノード探索してゲームオブジェクトに必要なデータを得る
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
//	case fbxsdk::FbxNodeAttribute::eMesh:	//メッシュを見つけた
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
//	//再帰処理
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

//頂点情報取得
void SetVert(FbxMesh* pMesh, vector<XMFLOAT4>& pVertex)
{
	int VertexCount = pMesh->GetControlPointsCount();	//頂点数
	FbxVector4* Vertices = pMesh->GetControlPoints();			//頂点インデックス配列
	vector<XMFLOAT4> Vertex;				//頂点情報
	int i;
	Vertex.resize(VertexCount);

	for (i = 0; i < VertexCount; ++i)
	{
		//頂点座標取得 
		Vertex[i].x = (FLOAT)Vertices[i][0];
		Vertex[i].y = (FLOAT)Vertices[i][1];
		Vertex[i].z = (FLOAT)Vertices[i][2];
		Vertex[i].w = (FLOAT)Vertices[i][3];
	}

	pVertex = Vertex;
}

//ポリゴンのインデックス取得
void SetIndex(FbxMesh* pMesh, vector<int>& pIndex)
{
	if (pMesh == NULL)
	{
		return;
	}

	int PolygonNum = pMesh->GetPolygonCount();					//ポリゴン数
	int PolygonVertexNum = pMesh->GetPolygonVertexCount();		//ポリゴンの頂点インデックス数
	int* Vertics = pMesh->GetPolygonVertices();					//ポリゴンの頂点インデックス配列
	vector<int> Poly;
	Poly.resize(PolygonNum * 3);
	
	//MeshData->m_pMesh->Index.resize(PolygonNum);
	int LayerCount = pMesh->GetLayerCount();

	int PolyInMaterial = pMesh->GetPolygonCount();
	int PolyNum=0;
	
	for (int i = 0; i < PolyInMaterial;i++)
	{
		int IndexNumInPoly = pMesh->GetPolygonSize(i);			//i番目のポリゴンの頂点数

		for (int j = 0; j < IndexNumInPoly; j++)
		{
			Poly[PolyNum + j] = pMesh->GetPolygonVertex(i, j);	//i番目のポリゴンのj番目の頂点のインデックス番号
		}

		PolyNum += IndexNumInPoly;
	}
	
	pIndex = Poly;
}

//法線取得
void SetNormal(FbxLayer* Layer ,vector<XMFLOAT3>& pNormal)
{
	//法線データの取得
	vector<XMFLOAT3> Normal;

	FbxLayerElementNormal* NormalElem = Layer->GetNormals();
	if (NormalElem == NULL)
	{
		return;   // 法線無し
	}

	int    NormalNum = NormalElem->GetDirectArray().GetCount();
	int    IndexNum = NormalElem->GetIndexArray().GetCount();
	Normal.resize(NormalNum);

	//マッピングモード
	FbxLayerElement::EMappingMode   NormalMapMode = NormalElem->GetMappingMode();
	//取得方法
	FbxLayerElement::EReferenceMode NormalRefMode = NormalElem->GetReferenceMode();

	if (NormalMapMode == FbxLayerElement::eByPolygonVertex)
	{
		if (NormalRefMode == FbxLayerElement::eDirect)
		{
			// 直接取得
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
			// 直接取得
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
	//UV取得
	FbxLayerElementUV* UVElem = pLayer->GetUVs();

	int UVCount = UVElem->GetDirectArray().GetCount();	//UV座標の数
	int UVindexCount = UVElem->GetIndexArray().GetCount();//UVインデックスの数
	int Size = UVCount > UVindexCount ? UVCount : UVindexCount;	//上二つの大きい方をサイズに
	vector<XMFLOAT2> UV;
	UV.resize(Size);
	vector<int> s;
	
	//マッピングモード
	FbxLayerElement::EMappingMode UVMapMode = UVElem->GetMappingMode();
	//取得方法
	FbxLayerElement::EReferenceMode RefMode = UVElem->GetReferenceMode();

	if (UVMapMode == FbxLayerElement::eByPolygonVertex)
	{
		FbxArray<FbxVector2> UVArray;
		UVElem->GetDirectArray().CopyTo(UVArray);

		if (RefMode == FbxLayerElement::eDirect)
		{
			// 直接取得
			for (int j = 0; j < Size; ++j) {
				UV[j].x = (float)UVArray.GetAt(j)[0];
				UV[j].y = (float)UVArray.GetAt(j)[1];
			}
		}
		else if (RefMode == FbxLayerElement::eIndexToDirect)
		{
			// インデックスから取得
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

//メッシュの取得
void MeshFactory::CreateMesh(FbxMesh* pMesh, shared_ptr<vector<MyVertex>> pVertex)
{
	int PolygonNum = pMesh->GetPolygonCount();			//ポリゴン数
	int VertexCount = pMesh->GetControlPointsCount();	//頂点数
	if (!(PolygonNum > 1))
		return;
	
	vector<XMFLOAT4>	Vertex;		//頂点座標
	vector<int>			Poly;		//ポリゴンインデックス
	vector<XMFLOAT3>	Normal;		//法線ベクトル
	vector<XMFLOAT2>	UV;			//UV座標

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

	//UVと法線を頂点位置のインデックスでソート
	int PolygonVertexNum = Poly.size();
	//法線取得
	SetNormal(Layer, Normal);

	vector<XMFLOAT3> SortNorm;
	SortNorm.resize(VertexCount);

	for (int i = 0; i < PolygonNum; i++)
		SortNorm[Poly[i]] = Normal[i];

	//UV座標取得
	SetUV(Layer, UV);

	vector<XMFLOAT2> SortUV;
	SortUV.resize(VertexCount);

	for (int i = 0; i < PolygonNum; i++)
		SortUV[Poly[i]] = UV[i];

	//データ格納
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
	int* Vertics = pMesh->GetPolygonVertices();					//ポリゴンの頂点インデックス配列

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
	//アンビエントカラー
	Material->Ambient.x = Lambert->Ambient.Get()[0];
	Material->Ambient.y = Lambert->Ambient.Get()[1];
	Material->Ambient.z = Lambert->Ambient.Get()[2];
	Material->Ambient.w = 1.0;

	//ディフューズ
	Material->Diffuse.x = Lambert->Diffuse.Get()[0];
	Material->Diffuse.y = Lambert->Diffuse.Get()[1];
	Material->Diffuse.z = Lambert->Diffuse.Get()[2];
	

	////エミッシブ
	//Material->Emissive.x = Lambert->Emissive.Get()[0];
	//Material->Emissive.y = Lambert->Emissive.Get()[1];
	//Material->Emissive.z = Lambert->Emissive.Get()[2];

	////バンプ
	//Material->Bump.x = Lambert->Bump.Get()[0];
	//Material->Bump.y = Lambert->Bump.Get()[1];
	//Material->Bump.z = Lambert->Bump.Get()[2];

	//透過
	Material->Alpha = 1.0f;
}

void SetPhongInfo(FbxSurfacePhong* Phong, MyPhong* Material)
{
	//ランバートと同じ情報を取得
	SetLambertInfo(Phong, Material);

	//スペキュラ
	Material->Speculer.r = Phong->Specular.Get()[0];
	Material->Speculer.g = Phong->Specular.Get()[1];
	Material->Speculer.b = Phong->Specular.Get()[2];
	//光沢
	Material->Shiness = Phong->Shininess.Get();
	//反射光
	Material->Refrect = Phong->ReflectionFactor.Get();
}

void SetMaterial()
{
}

void MeshFactory::CreateMaterial(FbxMesh* pMesh, MyLambert& MaterialData, vector<ComPtr<ID3D11ShaderResourceView>>& pTex,int MeshNum)
{
	FbxNode* Node = pMesh->GetNode();			//メッシュのノード取得

	if (Node == NULL)
	{
		return;
	}

	int MaterialNum = Node->GetMaterialCount();	//マテリアル数
	if (MaterialNum == 0)
	{
		return;
	}
	//マテリアル格納
	MyLambert	DummyMaterial;

	FbxSurfaceMaterial* Material = Node->GetMaterial(MeshNum);
	
	if (Material == NULL)
	{
		return;
	}

	if (Material->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		//ランバートマテリアル
		FbxSurfaceLambert* Lambert = (FbxSurfaceLambert*)Material;
		SetLambertInfo(Lambert, &MaterialData);
	}
	else if (Material->GetClassId().Is(FbxSurfacePhong::ClassId))
	{
		//フォングマテリアル
		FbxSurfacePhong* Phong = (FbxSurfacePhong*)Material;
	}
	else
	{
		//その他
		SetLambertInfo(NULL, &MaterialData);
	}

	FbxProperty DiffuseProperty = Material->FindProperty(FbxSurfaceMaterial::sDiffuse);
	int layerNum = DiffuseProperty.GetSrcObjectCount(FbxCriteria::ObjectType(FbxLayeredTexture::ClassId));

	//テクスチャ取得
	if (layerNum > 0)
	{
		//レイヤードテクスチャ
	}
	else
	{
		//テクスチャ一つ
		int TexNum = DiffuseProperty.GetSrcObjectCount(FbxCriteria::ObjectType(FbxTexture::ClassId));
		if (TexNum == 0)
			return;

		//pTex.resize(TexNum);
			
		FbxFileTexture* FileTex = FbxCast<FbxFileTexture>(DiffuseProperty.GetSrcObject(FbxCriteria::ObjectType(FbxTexture::ClassId),0));
		if (FileTex == NULL)
			return;
		FbxString Path = FbxPathUtils::GetFileName(FileTex->GetFileName());
		const char* TexPath = Path;

		//相対パスへ変換
		char RelativePath[255] = "../ShaderPractice/Resource/";
		strcat_s(RelativePath, 255, TexPath);

		//パスからテクスチャを取得
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

//アニメーション取得
void MeshFactory::CreateAnimInfo(FbxMesh* pMesh,vector<RefBone>& pBone, vector<MySkinAnimation>& pAnim)
{
	FbxNode* pNode = pMesh->GetNode();		//ノード
	FbxScene* AnimScene = pMesh->GetScene();//シーン

	FbxTime::EMode TimeMode;				//タイムモード取得
	TimeMode = AnimScene->GetGlobalSettings().GetTimeMode();

	FbxTime Period;							//1フレームの時間
	Period.SetTime(0, 0, 0, 1, 0, TimeMode);

	FbxArray<FbxString*> NameArray;			//アニメーション名
	AnimScene->FillAnimStackNameArray(NameArray);
	int AnimNum = NameArray.GetCount();		//アニメーション数

	FbxTakeInfo* TakeInfo = new FbxTakeInfo;//テイクの情報
	FbxTime Start;							//アニメーションスタート時間
	FbxTime Stop;							//終了時間
	int startFrame;							//アニメーションスタートフレーム
	int stopFrame;							//終了フレーム
	FbxMatrix MatrixData;					//姿勢
	vector<MySkinAnimation> Animation;
	Animation.resize(AnimNum);

	for (int i = 0; i < AnimNum; i++)
	{
		TakeInfo = AnimScene->GetTakeInfo(*(NameArray[i]));
		if (TakeInfo)
		{
			//continue;
		}

		//1テイクの開始、終了時間取得
		Start = TakeInfo->mLocalTimeSpan.GetStart();
		Stop  = TakeInfo->mLocalTimeSpan.GetStop();
		Animation[i].Start = (int)(Start.Get() / Period.Get());
		Animation[i].Stop = (int)(Stop.Get() / Period.Get());

		Animation[i].m_AnimMatrix.resize(Animation[i].Stop - Animation[i].Start);
		//主ボーンの姿勢取得
		for (int j = 0; j < Animation[i].Stop-Animation[i].Start; j++)
		{
			MatrixData = pNode->EvaluateGlobalTransform(Period*j);
			CastMatrix(&Animation[i].m_AnimMatrix[j], MatrixData);
		}
		//ボーンの情報取得
		CreateBoneInfo(pMesh, pBone,Animation[i], Animation[i].Start, Animation[i].Stop);

		////アニメーションコントローラにセット
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
	RefBone Bone;			//ボーン
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

	FbxAMatrix  InitMat;	//初期姿勢						
	FbxMatrix	FrameMat;	//フレーム姿勢
	XMMATRIX	CastMat;	//変換用姿勢

	pCluster->GetTransformLinkMatrix(InitMat);
	CastMatrix(&pSkelton->m_Bone[ClusterNum].m_InitMat, InitMat);
	XMMatrixInverse(pSkelton->m_Bone[ClusterNum].m_InitMat.r, pSkelton->m_Bone[ClusterNum].m_InitMat);
	pAnim.m_AnimBoneMatrix[ClusterNum].reserve(StopFrame - StartFrame);

	//各フレームの姿勢取得
	for (int i = 0; i < StopFrame-StartFrame; i++)
	{
		FrameMat = pCluster->GetLink()->EvaluateGlobalTransform(i);
		CastMatrix(&CastMat, FrameMat);
		pAnim.m_AnimBoneMatrix[ClusterNum].push_back(CastMat);
	}
}

//ボーンの情報取得
void MeshFactory::CreateBoneInfo(FbxMesh* pMesh, vector<RefBone>& pBone,MySkinAnimation& pAnim, int StartFrame,int StopFrame)
{
	int SkinCount= pMesh->GetDeformerCount(FbxDeformer::eSkin);	//スキン数
	FbxSkin* pSkin;												//スキン
	int ClusterNum;											//クラスタ数
	FbxCluster* pCluster;										//クラスタ
	
	for (int i = 0; i < SkinCount; i++)
	{
		//スキンを取得
		pSkin = (FbxSkin*)pMesh->GetDeformer(i, FbxDeformer::eSkin);
		ClusterNum = pSkin->GetClusterCount();
		//ボーンの取得
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

//スプライト作成
bool MeshFactory::CreateSprite(const char* FileName, shared_ptr<Sprite>& pSprite, RECT Rect, RECT Size)
{
	MyMesh Mesh;
	//スプライトの頂点定義
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

	//スプライトの頂点インデックス定義
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

	//テクスチャ
	ID3D11ShaderResourceView* Tex;

	D3DX11CreateShaderResourceViewFromFile(m_pDevice->GetDevice(), FileName, NULL, NULL, &Tex, NULL);
	//m_pTexture->Textures[FileName] = Tex;
	pSprite = make_shared<Sprite>(Mesh, Tex);
	pSprite->m_Material.Alpha = 1.0;

	//バッファ作成
	pSprite->CreateVertBuffer(m_pDevice->GetDevice());
	pSprite->CreateIndexBuffer(m_pDevice->GetDevice());
	pSprite->CreateMaterialBuffer(m_pDevice->GetDevice());

	//テクスチャサンプラ
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