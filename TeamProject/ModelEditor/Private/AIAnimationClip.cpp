#include "AIAnimationClip.h"
#include "AIModelData.h"
#include "AIChannel.h"

CAIAnimationClip::CAIAnimationClip()
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

	_int lowestBone = INT_MAX;
	_int RootChan = -1;
	for (_uint i = 0; i < m_iNumChannels; i++)
	{
		_int iBoneIndex = pAIModelData->Find_BoneIndexByName(m_Channels[i]->Get_Name());
		lowestBone = min(lowestBone, iBoneIndex);

		if (lowestBone == iBoneIndex)
			RootChan = i;
	}

	if(-1 != RootChan)
		dynamic_cast<CAIChannel*>(m_Channels[RootChan])->Set_Root();

	return S_OK;
}

void CAIAnimationClip::Save_File(ofstream& ofs, const _float4x4* WorldMatrix)
{
	ANIMATION_CLIP_HEADER tClipHeader{};
	strcpy_s(tClipHeader.ClipName, sizeof(tClipHeader.ClipName), m_ClipName.c_str());
	tClipHeader.fDuration = m_fDuration;
	tClipHeader.fTickPerSecond = m_fTickPerSecond;
	tClipHeader.iNumChannels = m_iNumChannels;

	ofs.write(reinterpret_cast<const char*>(&tClipHeader), sizeof(tClipHeader));

	for (_uint i = 0; i < m_iNumChannels; i++) {
		static_cast<CAIChannel*>(m_Channels[i])->Save_File(ofs, WorldMatrix);
	}
}

CAIAnimationClip* CAIAnimationClip::Create(const aiAnimation* pAIAnimation, CAIModelData* pAIModelData)
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

