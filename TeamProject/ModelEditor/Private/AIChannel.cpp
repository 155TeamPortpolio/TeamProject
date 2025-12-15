#include "AIChannel.h"
#include "AIModelData.h"

CAIChannel::CAIChannel()
{
}

HRESULT CAIChannel::Initialize(const aiNodeAnim* pAIChannel, CAIModelData* pAIModelData)
{
    m_ChannelName = pAIChannel->mNodeName.data;
    m_iNumKeyFrames = max(max(pAIChannel->mNumPositionKeys, pAIChannel->mNumRotationKeys), pAIChannel->mNumScalingKeys);

    _int Index = pAIModelData->Find_BoneIndexByName(m_ChannelName);
    if (Index == -1)
        return E_FAIL;

	m_iBoneIndex = static_cast<_uint> (pAIModelData->Find_BoneIndexByName(m_ChannelName));
	m_ChannelName = pAIModelData->Find_BoneNameByIndex(m_iBoneIndex);
	m_isRoot = pAIModelData->isRootBone(m_iBoneIndex);

	_float3         vScale = {};
	_float4         vRotation = {};
	_float3         vTranslation = {};

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME            KeyFrame{};

		if (i < pAIChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = pAIChannel->mScalingKeys[i].mTime;
		}

		if (i < pAIChannel->mNumRotationKeys)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			KeyFrame.fTrackPosition = pAIChannel->mRotationKeys[i].mTime;
		}

		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vTranslation;

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

void CAIChannel::Save_File(ofstream& ofs)
{
	ANIMATION_CHANNEL_HEADER tChannelHeader{};
	tChannelHeader.iBoneIndex = m_iBoneIndex;
	tChannelHeader.iNumKeyFrames = m_KeyFrames.size();
	tChannelHeader.isRootBoneChannel = m_isRoot;
	strcpy_s(tChannelHeader.BoneName, sizeof(tChannelHeader.BoneName), m_ChannelName.c_str());

	ofs.write(reinterpret_cast<const char*>(&tChannelHeader), sizeof(tChannelHeader));

	for (auto& keyFrame : m_KeyFrames) {
		ofs.write(reinterpret_cast<const char*>(&keyFrame), sizeof(KEYFRAME));
	}
}

CAIChannel* CAIChannel::Create(const aiNodeAnim* pAIChannel, CAIModelData* pAIModelData)
{
    CAIChannel* pInstance = new CAIChannel();

    if (FAILED(pInstance->Initialize(pAIChannel, pAIModelData))) {
        MSG_BOX("Create Failed : Engine | CAIChannel");
        return nullptr;
    }

    return pInstance;
}

void CAIChannel::Free()
{
	__super::Free();
}
