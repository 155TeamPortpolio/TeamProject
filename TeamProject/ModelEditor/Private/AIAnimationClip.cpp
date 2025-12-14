#include "AIAnimationClip.h"
#include "AIChannel.h"

CAIAnimationClip::CAIAnimationClip()
	: CAnimationClip{}
{
}

HRESULT CAIAnimationClip::Initialize(const aiAnimation* pAIAnimation, CAIModelData* pAIModelData)
{
	m_iNumChannels = pAIAnimation->mNumChannels;
	m_fDuration = pAIAnimation->mDuration;
	m_fTickPerSecond = pAIAnimation->mTicksPerSecond;
	m_ClipName = pAIAnimation->mName.C_Str();

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CAIChannel* pChannel = CAIChannel::Create(pAIAnimation->mChannels[i], pAIModelData);
		if (pChannel)
			m_Channels.push_back(pChannel);
	}
	m_iNumChannels = m_Channels.size();
	return S_OK;
}
}

void CAIAnimationClip::Save_File(ofstream& ofs)
{
}

CAnimationClip* CAIAnimationClip::Create(const aiAnimation* pAIAnimation, CAIModelData* pAIModelData)
{
    CAIAnimationClip* pInstance = new CAIAnimationClip();

    if (FAILED(pInstance->Initialize(pAIAnimation, pAIModelData))) {
        MSG_BOX("Create Failed : Engine | CAIAnimationClip");
        return nullptr;
    }

    return pInstance;
}

void CAIAnimationClip::Free()
{
	__super::Free();
}

