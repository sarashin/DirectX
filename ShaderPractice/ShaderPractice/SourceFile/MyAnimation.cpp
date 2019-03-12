#include"stdafx.h"
#include"MyAnimation.h"

MySkinAnimation::MySkinAnimation()
{
	m_Skelton = make_shared<MySkelton>();
}

MySkinAnimation::~MySkinAnimation()
{
	m_Skelton.reset();
	m_AnimBoneMatrix.clear();
	m_AnimMatrix.clear();
}
//void MySkelton::UpdateBoneMat(int Frame)
//{
//	for (int i = 0; i < BoneCount; i++)
//	{
//		m_BoneMat[i] = m_Bone[i].BoneMat[Frame];
//	}
//}

//XMMATRIX* MySkelton::GetBoneMat()
//{
//	return &m_BoneMat[0];
//}
//
//void MySkelton::SetBone(MyBone* Bone)
//{
//	if (Bone == NULL)
//	{
//		return;
//	}
//	m_Bone.push_back(*Bone);
//}
//
//MyBone* MySkelton::GetBone()
//{
//	return &m_Bone[0];
//}
//
int MySkelton::GetBoneNum()
{
	return m_Bone.size();
}

////ボーン数取得
//int MySkinAnimation::GetBoneNum()
//{
//	return m_Skelton->GetBoneNum();
//}

MyAnimationContainer::MyAnimationContainer()
{
	m_Mesh = make_shared<MyMeshData>();
}

MyAnimationContainer::MyAnimationContainer(vector<MySkinAnimation> Anim)
{
	m_Animation = Anim;
}
//
////アニメーションの作成
//void MyAnimationContainer::SetAnim(MySkinAnimation* Anim,string Name)
//{
//	if (Anim != NULL)
//	{
//		return;
//	}
//
//	if (m_AnimIndex[Name] != NULL)
//	{
//		return;
//	}
//
//	m_Animation.push_back(*Anim);
//	m_AnimIndex[Name] = m_AnimIndex.size();
//}
//
////アニメーションの姿勢取得
//void MyAnimationContainer::GetAnimMat(XMMATRIX* OptMat)
//{
//	int BoneNum = m_Animation[NowAnimNum].m_Skelton->GetBoneNum();
//	OptMat = new XMMATRIX[BoneNum];
//
//	OptMat = m_Animation[NowAnimNum].m_Skelton->GetBoneMat().data();
//	
//	Frame++;
//}
