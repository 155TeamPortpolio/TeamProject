#include "Engine_Defines.h"
#include "AudioSource.h"
#include "SoundData.h"
#include "GameInstance.h"
#include "IResourceService.h"
#include "IAudioService.h"
#include "GameObject.h"
#include "Transform.h"
CAudioSource::CAudioSource()
	:m_pAudioDevice(CGameInstance::GetInstance()->Get_AudioDev())
{
}

CAudioSource::CAudioSource(const CAudioSource& rhs)
	:m_pAudioDevice(CGameInstance::GetInstance()->Get_AudioDev())
	, CComponent(rhs), m_Audios(rhs.m_Audios)
{
	for (auto& sound : m_Audios)
		Safe_AddRef(sound.second.pSound);
}

CAudioSource::~CAudioSource()
{
}

HRESULT CAudioSource::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAudioSource::Initialize(COMPONENT_DESC* pArg)
{
	m_pTransform = m_pOwner->Get_Component<CTransform>();
	return S_OK;
}

HRESULT CAudioSource::Add_Slot(const string& levelTag, const string& SoundKey, const string& slotKey, bool isLoop, SOUND_GROUP eGroup)
{
	if (m_Audios.count(SoundKey)) {
		return E_FAIL;
	};

	IResourceService* pService = CGameInstance::GetInstance()->Get_ResourceMgr();
	AUDIO_SLOT audioSlot = {};
	audioSlot.pSound = pService->Load_Sound(levelTag, SoundKey);
	audioSlot.isInfinite = isLoop;
	audioSlot.eGroup = eGroup;

	if (!audioSlot.pSound)
		return E_FAIL;

	Safe_AddRef(audioSlot.pSound);

	auto [it, inserted] = m_Audios.emplace(slotKey.empty() ? SoundKey : slotKey, audioSlot);
	if (!inserted)
	{
		MSG_BOX("There is Same Key Audio : CAudioSource");
		Safe_Release(audioSlot.pSound);
		return E_FAIL;
	}

	return S_OK;

}

HRESULT CAudioSource::Add_Slot(const string& levelTag, const string& SoundKey, const string& slotKey, bool isLoop, SOUND_GROUP eGroup, _float sound)
{
	IResourceService* pService = CGameInstance::GetInstance()->Get_ResourceMgr();
	AUDIO_SLOT audioSlot = {};
	audioSlot.pSound = pService->Load_Sound(levelTag, SoundKey);
	audioSlot.isInfinite = isLoop;
	audioSlot.eGroup = eGroup;
	audioSlot.fVolume = sound;

	if (!audioSlot.pSound)
		return E_FAIL;

	Safe_AddRef(audioSlot.pSound);

	auto [it, inserted] = m_Audios.emplace(slotKey.empty() ? SoundKey : slotKey, audioSlot);
	if (!inserted)
	{
		MSG_BOX("There is Same Key Audio : CAudioSource");
		Safe_Release(audioSlot.pSound);
		return E_FAIL;
	}

	return S_OK;
}

void CAudioSource::Set_SlotVolume(const string& slotKey, _float fVolume)
{
	auto iter = m_Audios.find(slotKey);
	if (iter == m_Audios.end())
		return;

	AUDIO_SLOT& slot = iter->second;
	slot.fVolume = fVolume;
	bool isPlaying = false;

	if (iter->second.pChanel->isPlaying(&isPlaying) != FMOD_OK || !isPlaying)
		return;
	iter->second.pChanel->setVolume(slot.fVolume);
}

void CAudioSource::Set_SlotLoopCount(const string& slotKey, _int iLoopCount)
{
	auto iter = m_Audios.find(slotKey);
	if (iter == m_Audios.end())
		return;

	AUDIO_SLOT& slot = iter->second;
	slot.iLoopCount = iLoopCount;
}

void CAudioSource::Set_SlotPuase(const string& slotKey, _bool isPaused)
{
	auto iter = m_Audios.find(slotKey);
	if (iter == m_Audios.end())
		return;

	AUDIO_SLOT& slot = iter->second;

	if (!slot.pChanel)
		return;
	slot.pChanel->setPaused(isPaused);
	slot.isPaused = isPaused;
}

void CAudioSource::Set_3DAttribute(const string& slotKey, _bool _3DAttribute)
{
	auto iter = m_Audios.find(slotKey);
	if (iter == m_Audios.end())
		return;

	AUDIO_SLOT& slot = iter->second;
	slot.is3DAttribute = _3DAttribute;
}

void CAudioSource::FadeOut_Volume(const string& slotKey, _float factor)
{
	auto iter = m_Audios.find(slotKey);
	if (iter == m_Audios.end())
		return;

	if (!iter->second.pChanel) return;

	bool isPlaying = false;

	if (iter->second.pChanel->isPlaying(&isPlaying) != FMOD_OK || !isPlaying)
		return;
	float vol = 1.f;
	iter->second.pChanel->getVolume(&vol);

	float newVol = vol * factor;

	if (newVol < 0.02f) {
		iter->second.pChanel->stop();
		iter->second.pChanel = nullptr;
		iter->second.isPaused = false;
	}
	else
	iter->second.pChanel->setVolume(newVol);
}

void CAudioSource::FadeIn_Volume(const string& slotKey, _float step, _float dst)
{
	auto iter = m_Audios.find(slotKey);
	if (iter == m_Audios.end())
		return;

	auto& slot = iter->second;

	if (!slot.pChanel)
		return;

	bool isPlaying = false;
	if (slot.pChanel->isPlaying(&isPlaying) != FMOD_OK || !isPlaying)
		return;

	float vol = 0.f;
	slot.pChanel->getVolume(&vol);

	float newVol = vol + step;   

	if (newVol >= dst)
	{
		newVol = dst;
		slot.pChanel->setVolume(newVol);
	}
	else
	{
		slot.pChanel->setVolume(newVol);
	}
}


void CAudioSource::Play(const string& SoundKey)
{
	auto iter = m_Audios.find(SoundKey);
	if (iter == m_Audios.end())
		return;

	AUDIO_SLOT& slot = iter->second;

	_float now = CGameInstance::GetInstance()->Get_TimeMgr()->Get_TotalTime("Audio_Timer");

	if (now - slot.lastPlayTime < 0.05f)
		return;
	slot.lastPlayTime = now;

	AUDIO_PACKET packet{};
	packet.ppChannelToUpdate = &slot.pChanel;
	packet.pSound = slot.pSound;

	// 슬롯에서 설정한 값들 싹 복사
	packet.isInfinite = slot.isInfinite;
	packet.isPaused = slot.isPaused;
	packet.is3DAttribute = slot.is3DAttribute;
	packet.fVolume = slot.fVolume;
	packet.iLoopCount = slot.iLoopCount;  // isInfinite면 이 값은 무시하게

	// 그룹도 하나 정해서
	packet.eGroup = slot.eGroup;
	if (slot.isInfinite)
	{
		packet.iLoopCount = -1;
		packet.isInfinite = true;
	}
	else
	{
		packet.iLoopCount = slot.iLoopCount;
		packet.isInfinite = false;
	}

	packet.isPaused = slot.isPaused;

	XMStoreFloat4(&m_vPos, m_pTransform->Get_Pos());
	packet.vPosition = { m_vPos.x, m_vPos.y, m_vPos.z };

	m_pAudioDevice->Play(packet);
}
void CAudioSource::PlayOnce(const string& slotKey)
{
	_float now = CGameInstance::GetInstance()->Get_TimeMgr()->Get_TotalTime("Audio_Timer");

	auto it = m_Audios.find(slotKey);
	if (it == m_Audios.end()) return;

	AUDIO_SLOT& slot = it->second;

	// 최소 간격 (예: 0.04~0.07 사이 한 번 정도)
	float minInterval = 0.05f;
	if (now - slot.lastPlayTime < minInterval)
		return;

	slot.lastPlayTime = now;

	// 이전 채널 stop 안 함! (그냥 자연스럽게 tail 남게)
	// if (slot.pChanel) { slot.pChanel->stop(); ... } 이런 거 제거

	// 새로 재생
	AUDIO_PACKET packet{};
	packet.ppChannelToUpdate = &slot.pChanel;
	packet.pSound = slot.pSound;
	packet.is3DAttribute = false;
	packet.fVolume = slot.fVolume;
	packet.iLoopCount = 0;
	packet.eGroup = slot.eGroup;

	m_pAudioDevice->Play(packet);
}

void CAudioSource::RePlay(const string& slotKey)
{
	auto it = m_Audios.find(slotKey);
	if (it == m_Audios.end())
		return;

	AUDIO_SLOT& slot = it->second;
	FMOD::Channel* pChannel = slot.pChanel;

	bool needNewPlay = false;

	if (!pChannel)
	{
		// 채널 자체가 없으니 새로 재생
		needNewPlay = true;
	}
	else
	{
		bool isPlaying = false;
		if (pChannel->isPlaying(&isPlaying) != FMOD_OK || !isPlaying)
		{
			// 끝났거나 stop 된 상태 → 새로 재생
			needNewPlay = true;
		}
		else
		{
			bool paused = false;
			pChannel->getPaused(&paused);

			if (paused)
			{
				// 그냥 다시 이어서 재생
				pChannel->setPaused(false);
				slot.isPaused = false;
				return;
			}
			else
			{
				return;
			}
		}
	}

	if (needNewPlay)
	{
		AUDIO_PACKET packet{};
		packet.ppChannelToUpdate = &slot.pChanel;
		packet.pSound = slot.pSound;
		packet.is3DAttribute = slot.is3DAttribute;
		packet.fVolume = slot.fVolume;
		packet.iLoopCount = slot.iLoopCount;   // PlayOnce처럼 0으로 고정하고 싶으면 0
		packet.eGroup = slot.eGroup;

		m_pAudioDevice->Play(packet);

		// 타이머/쿨타임 같은 거 쓰고 싶으면 여기서 업데이트
		slot.lastPlayTime = CGameInstance::GetInstance()
			->Get_TimeMgr()->Get_TotalTime("Audio_Timer");
	}
}


void CAudioSource::Render_GUI()
{
	ImGui::SeparatorText("Audio Source");
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * (m_Audios.size() + 3)) + (ImGui::GetStyle().WindowPadding.y * 2);

	ImGui::BeginChild("##AudioSourceChild", ImVec2{ 0, childHeight }, true);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
	for (auto& pair : m_Audios)
	{
		if (ImGui::Button(pair.first.c_str())) {
			Play(pair.first);
		}
	}
	ImGui::PopStyleVar();
	ImGui::EndChild();
}

CAudioSource* CAudioSource::Create()
{
	CAudioSource* instance = new CAudioSource();
	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("AudioSource Comp Failed To Create : CAudioSource");
	}
	return instance;
}

CComponent* CAudioSource::Clone()
{
	return new CAudioSource(*this);
}

void CAudioSource::Free()
{
	__super::Free();

	for (auto& sound : m_Audios)
		Safe_Release(sound.second.pSound);

	m_Audios.clear();
}
