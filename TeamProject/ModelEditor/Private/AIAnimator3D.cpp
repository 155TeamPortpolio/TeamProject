#include "AIAnimator3D.h"
#include "AIAnimationClip.h"
#include "GameInstance.h"

CAIAnimator3D::CAIAnimator3D()
{
}

HRESULT CAIAnimator3D::Initialize(const aiScene* pAIScene, CAIModelData* pAIModelData)
{
    for (_uint i = 0; i < pAIScene->mNumAnimations; i++) {
        CAIAnimationClip* AIClip = CAIAnimationClip::Create(pAIScene->mAnimations[i], pAIModelData);
        if (!AIClip)
            return E_FAIL;
        
        m_pAnimClips.push_back(static_cast<CAnimationClip*>(AIClip));
    }

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
}
