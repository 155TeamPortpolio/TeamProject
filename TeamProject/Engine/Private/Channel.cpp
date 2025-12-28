#include "Engine_Defines.h"
#include "Channel.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(ifstream& ifs)
{
	ANIMATION_CHANNEL_HEADER channelHeader = {};
	ifs.read(reinterpret_cast<char*>(&channelHeader), sizeof(ANIMATION_CHANNEL_HEADER));

	m_ChannelName = channelHeader.BoneName;     //본과 일치할 수 있도록.
	m_iBoneIndex = channelHeader.iBoneIndex;
	m_iNumKeyFrames = channelHeader.iNumKeyFrames;
	m_isRoot = channelHeader.isRootBoneChannel;

	for (size_t i = 0; i < channelHeader.iNumKeyFrames; i++)
	{
		KEYFRAME keyFrame = {};
		ifs.read(reinterpret_cast<char*>(&keyFrame), sizeof(KEYFRAME));
		m_KeyFrames.push_back(keyFrame);
	}

	return S_OK;
}
/*현재 트랙 위치*/
HRESULT CChannel::TranslateAnimateMatrix(vector<_float4x4>& transfomationMatrices, _float CurrentTrackPosition, _bool Loop)
{
	_uint iCurrentKeyIndex = { m_iNumKeyFrames - 1 };
	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		if (i > 0 && !m_KeyFrames[i].IsBefore(CurrentTrackPosition)) {
			iCurrentKeyIndex = i - 1;
			break;
		}
	}

	_XMKeyFrame keyFrame = {};

	KEYFRAME LastKeyframe = m_KeyFrames.back();
	KEYFRAME firstKeyFrame = m_KeyFrames.front();
	if (CurrentTrackPosition >= LastKeyframe.fTrackPosition) {
		keyFrame.vScale = XMLoadFloat3(&LastKeyframe.vScale);
		keyFrame.vRotation = XMLoadFloat4(&LastKeyframe.vRotation);
		keyFrame.vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyframe.vTranslation), 1.0f);
	}

	else if (CurrentTrackPosition == 0) {
		keyFrame.vScale = XMLoadFloat3(&firstKeyFrame.vScale);
		keyFrame.vRotation = XMLoadFloat4(&firstKeyFrame.vRotation);
		keyFrame.vTranslation = XMVectorSetW(XMLoadFloat3(&firstKeyFrame.vTranslation), 1.0f);
	}

	else {
		keyFrame = m_KeyFrames[iCurrentKeyIndex].LerpKeyFram(m_KeyFrames[iCurrentKeyIndex + 1], CurrentTrackPosition);
	}

	XMStoreFloat3(&nowFrame.vTranslation, keyFrame.vTranslation);
	XMStoreFloat3(&nowFrame.vScale, keyFrame.vScale);
	XMStoreFloat4(&nowFrame.vRotation, keyFrame.vRotation);

	_matrix TransformationMatrix = XMMatrixAffineTransformation(keyFrame.vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), keyFrame.vRotation, keyFrame.vTranslation);
	XMStoreFloat4x4(&transfomationMatrices[m_iBoneIndex], TransformationMatrix);
	return S_OK;
}

void CChannel::Render_GUI()
{
	string key = "Channel : " + m_ChannelName + "(" + to_string(m_iBoneIndex) + ")";
	ImGui::Text(key.c_str());

	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();

		ImGui::Text("Position:  %.3f, %.3f, %.3f",
			nowFrame.vTranslation.x,
			nowFrame.vTranslation.y,
			nowFrame.vTranslation.z);

		ImGui::Text("Scale:     %.3f, %.3f, %.3f",
			nowFrame.vScale.x,
			nowFrame.vScale.y,
			nowFrame.vScale.z);

		ImGui::Text("Rotation:  %.3f, %.3f, %.3f, %.3f",
			nowFrame.vRotation.x,
			nowFrame.vRotation.y,
			nowFrame.vRotation.z,
			nowFrame.vRotation.w);

		ImGui::EndTooltip();
	}
}

CChannel* CChannel::Create(ifstream& ifs)
{
	CChannel* instance = new CChannel;

	if (FAILED(instance->Initialize(ifs))) {
		Safe_Release(instance);
	}
	return instance;
}

void CChannel::Free()
{
	__super::Free();
}
