#include "AIAnimator3D.h"
#include "AIAnimationClip.h"
#include "AIModelData.h"
#include "Helper_Func.h"
#include "GameInstance.h"

CAIAnimator3D::CAIAnimator3D()
{
}

HRESULT CAIAnimator3D::Initialize(const aiScene* pAIScene, CAIModelData* pAIModelData)
{
	m_pAnimClips.reserve(0);

    for (_uint i = 0; i < pAIScene->mNumAnimations; i++) {
        CAIAnimationClip* AIClip = CAIAnimationClip::Create(pAIScene->mAnimations[i], pAIModelData);
        if (!AIClip)
            return E_FAIL;
        
        m_pAnimClips.push_back(static_cast<CAnimationClip*>(AIClip));
    }

	m_pData = static_cast<CModelData*>(pAIModelData);
	Safe_AddRef(m_pData);
	_float4x4 IdentityMatrix;
	XMStoreFloat4x4(&IdentityMatrix, XMMatrixIdentity());

	m_TransformationMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	m_ManipulateMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	m_CombinedMatrices.resize(m_pData->Get_BoneCount(), IdentityMatrix);
	

	/*뼈 개수만큼 뼈의 로컬상태를 가져옴*/
	for (size_t i = 0; i < m_pData->Get_BoneCount(); i++)
	{
		m_TransformationMatrices[i] = m_pData->Get_TransformMatrix(i);
	}
	/*부모 뼈를 받을 수 있게 기본값으로 초기화*/
	for (size_t i = 0; i < m_pData->Get_BoneCount(); i++)
	{
		int parent = m_pData->Get_BoneParentIndex(i);

		if (parent == -1) {
			m_CombinedMatrices[i] = m_TransformationMatrices[i];
		}
		else {
			_matrix ParentCombine = XMLoadFloat4x4(&m_CombinedMatrices[parent]);
			_matrix MyTransformation = XMLoadFloat4x4(&m_TransformationMatrices[i]);
			XMStoreFloat4x4(&m_CombinedMatrices[i], MyTransformation * ParentCombine);
		}
	}

	Resize_Layer(1);

	return S_OK;
}

void CAIAnimator3D::Render_GUI()
{
	__super::Render_GUI();
}

HRESULT CAIAnimator3D::Save_Animation(const string& SavePath, const _float4x4* WorldMatrix)
{
	string AnimSavePath = SavePath + "\\Anim\\";
	std::filesystem::create_directories(AnimSavePath);

	ANIM_META Meta;

	for (auto& Clip : m_pAnimClips) {
		std::filesystem::path filePath = std::filesystem::path(AnimSavePath) / (Clip->Get_Name() + ".anim");
		std::ofstream ofs(filePath, std::ios::binary);

		static_cast<CAIAnimationClip*>(Clip)->Save_File(ofs);

		ofs.close();

		ANIM_CLIP tClip{};
		tClip.ClipTag = Clip->Get_Name();
		Meta.Clips.push_back(tClip);
	}

	Meta.PreTransform = *WorldMatrix;

	size_t pos = Meta.Clips.back().ClipTag.find("_Ani_");
	string Name = Meta.Clips.back().ClipTag.substr(0, pos);

	Helper::SaveJson<ANIM_META>(Meta, SavePath + Name + "_Meta.json");

	return S_OK;
}

CAIAnimator3D* CAIAnimator3D::Create(const aiScene* pAIScene, CAIModelData* pAIModelData)
{
    CAIAnimator3D* pInstance = new CAIAnimator3D();

    if (FAILED(pInstance->Initialize(pAIScene, pAIModelData))) {
        MSG_BOX("Create Failed : Engine | CAIAnimator3D");
        return nullptr;
    }

    return pInstance;
}

void CAIAnimator3D::Free()
{
	__super::Free();

	for (auto& Clip : m_pAnimClips) {
		Safe_Release(Clip);
	}
}
