#include "Engine_Defines.h"
#include "AudioSource.h"
#include "SoundData.h"
#include "GameInstance.h"
#include "IResourceService.h"
#include "IAudioService.h"
#include "GameObject.h"
#include "Transform.h"

CAudioSource::SlotBuilder CAudioSource::Slot(const string& slotKey)
{
	auto iter = m_Audios.find(slotKey);
	if (iter != m_Audios.end())
	{
		return SlotBuilder(*this, iter->second);
	}
	AUDIO_SLOT slot{};
	return SlotBuilder(*this,slot);
}

CAudioSource::AUDIO_SLOT& CAudioSource::SlotBuilder::Play()
{
	if (!ownerSlot.pSound)
		return ownerSlot;
	_float now = CGameInstance::GetInstance()->Get_TimeMgr()->Get_TotalTime("Audio_Timer");
	if (now - ownerSlot.lastPlayTime < 0.05f) 
		return ownerSlot;
	ownerSlot.lastPlayTime = now;
	AUDIO_PACKET packet{};
	packet.ppChannelToUpdate = &ownerSlot.pChanel;
	packet.pSound = ownerSlot.pSound;
	packet.isInfinite = ownerSlot.isInfinite;
	packet.isPaused = ownerSlot.isPaused;
	packet.is3DAttribute = ownerSlot.is3DAttribute;
	packet.fVolume = ownerSlot.fVolume;
	packet.iLoopCount = ownerSlot.iLoopCount; 
	packet.eGroup = ownerSlot.eGroup;
	
	if (ownerSlot.isInfinite)
	{
		packet.iLoopCount = -1;
		packet.isInfinite = true;
	}
	else
	{
		packet.iLoopCount = ownerSlot.iLoopCount;
		packet.isInfinite = false;
	}

	packet.isPaused = ownerSlot.isPaused;
	_vector3 pos = ownerRef.m_pTransform->Get_WorldPos();
	packet.vPosition = { pos.x, pos.y, pos.z };
	AudioDevice()->Play(packet);

	return ownerSlot;
}

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

HRESULT CAudioSource::Add_Slot(const string& levelTag, const string& SoundKey)
{
	AUDIO_SLOT audioSlot = {};
	audioSlot.pSound = ResourceManager()->Load_Sound(levelTag, SoundKey);

	if (!audioSlot.pSound)
		return E_FAIL;

	Safe_AddRef(audioSlot.pSound);

	auto [it, inserted] = m_Audios.emplace(SoundKey, audioSlot);
	if (!inserted)
	{
		MSG_BOX("There is Same Key Audio : CAudioSource");
		Safe_Release(audioSlot.pSound);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CAudioSource::SoundFolder(const string& levelTag, const string& SoundFolder, const string& extension)
{
	filesystem::path folderPath = filesystem::path(SoundFolder);
	error_code errorCode;
	if (!filesystem::exists(folderPath, errorCode) || errorCode)
		return E_FAIL;

	if (!filesystem::is_directory(folderPath, errorCode) || errorCode)
		return E_FAIL;

	auto IsAudioExtension = [extension](const filesystem::path& filePath) -> bool
		{
			string extension = filePath.extension().string();
			transform(extension.begin(), extension.end(), extension.begin(),
				[](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
			return (extension == extension);
		};

	unordered_set<string> slotKeyUsed;
	const filesystem::directory_options iterOptions =
		filesystem::directory_options::skip_permission_denied;

	for (filesystem::directory_iterator iter(folderPath, iterOptions, errorCode);
		iter != filesystem::directory_iterator();iter.increment(errorCode))
	{
		if (errorCode)
		{
			errorCode.clear();
			continue;
		}

		const filesystem::directory_entry& entry = *iter;

		if (!entry.is_regular_file(errorCode) || errorCode)
		{
			errorCode.clear();
			continue;
		}

		const filesystem::path& filePath = entry.path();
		if (!IsAudioExtension(filePath))
			continue;

		string soundKey = filePath.filename().string();
		if (!slotKeyUsed.insert(soundKey).second)
		{
			int suffixIndex = 2;
			string uniqueSlotKey;
			do
			{
				uniqueSlotKey = soundKey + "_" + to_string(suffixIndex++);
			} 
			while (!slotKeyUsed.insert(uniqueSlotKey).second);
			soundKey = uniqueSlotKey;
		}

		const HRESULT addResult =Add_Slot(levelTag, soundKey);
		if (FAILED(addResult))
			continue;
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
