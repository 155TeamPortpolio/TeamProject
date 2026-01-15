#include "pch.h"
#include "CamDirector.h"
#include "CamSequencePlayer.h"
#include "SequenceCam.h"
#include "CharacterController.h"
#include "GameInstance.h"
#include "OrbitCam.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CCamDirector)

CGameObject* CCamDirector::GetCamObj(CamType type) const
{
	return ObjectManager()->Request_Object(m_camHandles[ENUM(type)]);
}

CGameObject* CCamDirector::GetSeqObj() const
{
	return GetCamObj(CamType::Sequence);
}

_bool CCamDirector::Register(const string& key, const filesystem::path& path)
{
	SeqEntry entry{};
	entry.path = path;
	CamUtil::Load(entry.path, entry.seqDesc);
	m_seqs[key] = move(entry);
	return true;
}

void CCamDirector::UnRegister(const string& key)
{
	if (m_playing.active && m_playing.key == key)
		StopAll(m_playing.defaultBlendOutSec);

	m_seqs.erase(key);
}

void CCamDirector::Update(_float dt)
{
	if (!m_playing.active) return;

	auto seqObj    = GetSeqObj();
	auto seqPlayer = seqObj->Get_Component<CCamSequencePlayer>();

	if (m_playing.pendingStart)
	{
		m_playing.blendInRemain -= dt;

		if (m_playing.blendInRemain <= 0.f)
		{
			seqPlayer->Play();
			m_playing.pendingStart = false;
		}
		return;
	}

	seqPlayer->Update(dt);

	if (!seqPlayer->IsPlaying())
		StopAll(m_playing.defaultBlendOutSec);
}

_uint CCamDirector::RequestSequence(const string& key, _float blendInSec, _bool resetTime, _float blendOutSec)
{
	if (m_playing.active) StopAll(blendOutSec);

	auto& entry = m_seqs.at(key);

	auto seqObj = GetSeqObj();
	auto seqPlayer = seqObj->Get_Component<CCamSequencePlayer>();
	auto seqCam = seqObj->Get_Component<CCamera>();

	seqPlayer->SetSequence(&entry.seqDesc);

	if (entry.seqDesc.space == CamSpace::Local) seqPlayer->SetSpaceReference(m_spaceRefHandle);
	else seqPlayer->ClearSpaceReference();

	seqPlayer->SetApplyEnabled(true);

	if (resetTime) seqPlayer->SetTime(0.f);

	const _uint handle = CameraManager()->Push(seqCam, blendInSec);

	m_playing.handle = handle;
	m_playing.key = key;
	m_playing.active = true;
	m_playing.defaultBlendOutSec = blendOutSec;
	m_playing.pendingStart = (blendInSec > 0.f);
	m_playing.blendInRemain = blendInSec;

	m_playing.returnCamType = m_defaultReturnCamType;
	if (m_defaultReturnCamType != CamType::None) m_playing.returnCamHandle = GetCamHandle(m_defaultReturnCamType);

	if (m_playing.pendingStart) seqPlayer->Pause();
	else seqPlayer->Play();

	return handle;
}


_bool CCamDirector::StopRequest(_uint handle, _float blendOutSec, _bool resetTime)
{
	const Matrix outWorld = *CameraManager()->Get_InversedViewMatrix();

	Vector3    outPos = outWorld.Translation();
	Quaternion outRot = Quaternion::CreateFromRotationMatrix(outWorld);
	outRot.Normalize();

	auto seqObj = GetSeqObj();
	auto seqPlayer = seqObj->Get_Component<CCamSequencePlayer>();

	seqPlayer->SetApplyEnabled(false);
	seqPlayer->Stop(false);

	if (resetTime) seqPlayer->SetTime(0.f);

	if (m_playing.returnCamType != CamType::None)
	{
		auto returnObj = ObjectManager()->Request_Object(m_playing.returnCamHandle);

		if (m_playing.returnCamType == CamType::Orbit)
			static_cast<COrbitCam*>(returnObj)->SnapFromCamPose(outPos, outRot);
	}

	const _bool ok = CameraManager()->Pop(handle, blendOutSec);
	m_playing = {};
	return ok;
}


void CCamDirector::StopAll(_float blendOutSec)
{
	if (!m_playing.active) return;
	StopRequest(m_playing.handle, blendOutSec, true);
}