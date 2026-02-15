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
	auto it = m_Audios.find(slotKey);
	if (it != m_Audios.end())
		return SlotBuilder(*this, it->second);

	// 흔한 실수: 확장자 없이 호출
	if (slotKey.find('.') == string::npos)
	{
		string withExt = slotKey + ".wav";
		it = m_Audios.find(withExt);
		if (it != m_Audios.end())
			return SlotBuilder(*this, it->second);
	}

	return SlotBuilder(*this, EmptySlot);
}
CAudioSource::AUDIO_SLOT& CAudioSource::SlotBuilder::Play()
{
	if (!ownerSlot.pSound) return ownerSlot;

	const bool needFadeIn = ownerSlot.hasPendingFadeIn;

	ownerRef.Play(ownerSlot.Key, false, false);

	if (needFadeIn && ownerSlot.pChannels.front())
	{
		ownerRef.FadeIn_Volume(ownerSlot.Key, ownerSlot.pendingFadeInSec, ownerSlot.pendingFadeInDst);
		ownerSlot.hasPendingFadeIn = false;
	}

	return ownerSlot;
}

CAudioSource::CAudioSource()
	:m_pAudioDevice(CGameInstance::GetInstance()->Get_AudioDev())
{
}

CAudioSource::CAudioSource(const CAudioSource& rhs)
	:m_pAudioDevice(CGameInstance::GetInstance()->Get_AudioDev())
	, CComponent(rhs), m_Audios(rhs.m_Audios), m_Sequences(rhs.m_Sequences)
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
	return S_OK;
}

HRESULT CAudioSource::Add_Slot(const string& levelTag, const string& SoundKey)
{
	AUDIO_SLOT audioSlot = {};
	audioSlot.pSound = ResourceManager()->Load_Sound(levelTag, SoundKey);
	if (!audioSlot.pSound)
		return E_FAIL;

	Safe_AddRef(audioSlot.pSound);
	audioSlot.Key = SoundKey;

	auto [it, inserted] = m_Audios.emplace(SoundKey, audioSlot);
	if (!inserted)
	{
		//MSG_BOX("There is Same Key Audio : CAudioSource");
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

	auto IsAudioExtension = [wantExt = extension](const filesystem::path& filePath) -> bool
		{
			string fileExt = filePath.extension().string();
			transform(fileExt.begin(), fileExt.end(), fileExt.begin(),
				[](unsigned char ch) { return (char)std::tolower(ch); });

			string lowerWant = wantExt;
			transform(lowerWant.begin(), lowerWant.end(), lowerWant.begin(),
				[](unsigned char ch) { return (char)std::tolower(ch); });

			return fileExt == lowerWant;
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
		ResourceManager()->Add_ResourcePath(soundKey, filePath.string());
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
	FMOD::Channel* channel = iter->second.pChannels.front();
	if (!channel) return;

	bool isPlaying = false;
	if (channel->isPlaying(&isPlaying) != FMOD_OK || !isPlaying)
		return;

	ClearFadePoints(channel);
	channel->setVolume(slot.fVolume);
	
}
void CAudioSource::ClearFadePoints(FMOD::Channel* channel)
{
	if (!channel) return;

	FMOD::System* system = m_pAudioDevice->Get_System();
	if (!system) return;

	int sampleRate = 0;
	if (system->getSoftwareFormat(&sampleRate, 0, 0) != FMOD_OK || sampleRate <= 0) return;

	unsigned long long channelClock = 0, parentClock = 0;
	if (channel->getDSPClock(&channelClock, &parentClock) != FMOD_OK) return;

	const unsigned long long dspNow = parentClock; 
	const unsigned long long dspFar = dspNow + (unsigned long long)(30.0f * (float)sampleRate);
	channel->removeFadePoints(dspNow, dspFar);
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
	FMOD::Channel* channel = iter->second.pChannels.front();

	if (!channel)
		return;
	channel->setPaused(isPaused);
	slot.isPaused = isPaused;
}

void CAudioSource::Set_SlotStop(const string& slotKey)
{
	auto iter = m_Audios.find(slotKey);
	if (iter == m_Audios.end())
		return;

	AUDIO_SLOT& slot = iter->second;
	FMOD::Channel* channel = slot.pChannels.front();

	if (!channel)
		return;
	channel->stop();
}

void CAudioSource::Set_AudioPos(_vector3 pos, _vector3 velocity)
{
	m_vPos = { pos.x,pos.y,pos.z };
	m_vVelocity = { velocity.x,velocity.y,velocity.z };
	for (auto& sound : m_Audios) {
		auto& slot = sound.second;
		if (slot.is3DAttribute && slot.pSound) {
			for (size_t i = 0; i < slot.pChannels.size(); i++)
			{
				_bool isPlaying = { false };
				slot.pChannels[i]->isPlaying(&isPlaying);
				if (isPlaying)
					slot.pChannels[i]->set3DAttributes(&m_vPos, &m_vVelocity);
			}
		
		}
	}
}

void CAudioSource::Set_3DAttribute(const string& slotKey, _bool _3DAttribute)
{
	auto iter = m_Audios.find(slotKey);
	if (iter == m_Audios.end())
		return;

	AUDIO_SLOT& slot = iter->second;
	slot.is3DAttribute = _3DAttribute;
}

void CAudioSource::FadeOut_Volume(const string& slotKey, _float durationSec)
{
	auto it = m_Audios.find(slotKey);
	if (it == m_Audios.end()) return;

	auto& slot = it->second;
	if (slot.pChannels.empty()) return;

	FMOD::Channel* channel = slot.pChannels.front();
	if (!channel) return;

	bool isPlaying = false;
	if (channel->isPlaying(&isPlaying) != FMOD_OK || !isPlaying) return;

	FMOD::System* system = m_pAudioDevice->Get_System();
	if (!system) return;

	int sampleRate = 0;
	if (system->getSoftwareFormat(&sampleRate, 0, 0) != FMOD_OK || sampleRate <= 0) return;

	unsigned long long channelClock = 0, parentClock = 0;
	if (channel->getDSPClock(&channelClock, &parentClock) != FMOD_OK) return;

	const float safeDuration = max(0.05f, (float)durationSec);
	const unsigned long long dspNow = parentClock;
	const unsigned long long dspEnd = dspNow + (unsigned long long)(safeDuration * (double)sampleRate);

	const unsigned long long dspFar = dspNow + (unsigned long long)(30.0 * (double)sampleRate);
	channel->removeFadePoints(dspNow, dspFar);

	float currentVol = 1.f;
	channel->getVolume(&currentVol);

	channel->addFadePoint(dspNow, currentVol);
	channel->addFadePoint(dspEnd, 0.0f);

	channel->setDelay(0, dspEnd, true);

	slot.hasStopScheduled = true;
	slot.stopDspClock = dspEnd;
}
void CAudioSource::FadeIn_Volume(const string& slotKey, _float durationSec, _float dstVol)
{
	auto it = m_Audios.find(slotKey);
	if (it == m_Audios.end()) return;

	auto& slot = it->second;

	// ★ front 채널만 사용
	if (slot.pChannels.empty())
	{
		// 재생 중이 아니면 먼저 재생
		Play(slotKey, false, true);
		if (slot.pChannels.empty())
			return;
	}

	FMOD::Channel* channel = slot.pChannels.front();
	if (!channel) return;

	bool isPlaying = false;
	if (channel->isPlaying(&isPlaying) != FMOD_OK || !isPlaying)
		return;

	FMOD::System* system = m_pAudioDevice->Get_System();
	if (!system) return;

	int sampleRate = 0;
	if (system->getSoftwareFormat(&sampleRate, 0, 0) != FMOD_OK || sampleRate <= 0)
		return;

	unsigned long long channelClock = 0, parentClock = 0;
	if (channel->getDSPClock(&channelClock, &parentClock) != FMOD_OK)
		return;

	const unsigned long long dspNow = parentClock;
	const float safeDuration = max(0.05f, (float)durationSec);
	const unsigned long long dspEnd =
		dspNow + (unsigned long long)(safeDuration * (float)sampleRate);

	// 기존 페이드 제거
	const unsigned long long dspFar =
		dspNow + (unsigned long long)(30.0f * (float)sampleRate);
	channel->removeFadePoints(dspNow, dspFar);

	// 0 → dstVol 페이드
	channel->setVolume(0.0f);
	channel->addFadePoint(dspNow, 0.0f);
	channel->addFadePoint(dspEnd, (float)dstVol);

	channel->setPaused(false);
}

void CAudioSource::Play(const string& soundKey, _bool continuePlay, _bool startPaused)
{
	auto it = m_Audios.find(soundKey);
	if (it == m_Audios.end()) return;

	AUDIO_SLOT& slot = it->second;
	if (!slot.pSound) return;

	_float now = CGameInstance::GetInstance()->Get_TimeMgr()->Get_TotalTime("Audio_Timer");

	// --------------------------------------
	// stop 예약이 걸려있으면 "front 채널"만 정리
	// --------------------------------------
	if (slot.hasStopScheduled)
	{
		if (!slot.pChannels.empty())
		{
			FMOD::Channel* frontChannel = slot.pChannels.front();
			if (frontChannel)
				frontChannel->stop();
			slot.pChannels.erase(slot.pChannels.begin()); // front 제거
		}

		slot.hasStopScheduled = false;
		slot.stopDspClock = 0;
		continuePlay = false;
	}

	// --------------------------------------
	// 겹침(one-shot) : 계속 벡터로 쌓기
	// --------------------------------------
	const bool wantOverlap = (!continuePlay) && (!slot.isInfinite);
	if (wantOverlap)
	{
		if (now - slot.lastPlayTime < 0.0017f)
			return;

		slot.lastPlayTime = now;
		Update_Audio(slot);

		FMOD::Channel* newChannel = nullptr;

		AUDIO_PACKET packet{};
		packet.ppChannelToUpdate = &newChannel;
		packet.pSound = slot.pSound;
		packet.isInfinite = false;
		packet.isPaused = startPaused;
		packet.is3DAttribute = slot.is3DAttribute;
		packet.fVolume = slot.fVolume;
		packet.iLoopCount = slot.iLoopCount;
		packet.eGroup = slot.eGroup;
		packet.vPosition = &m_vPos;

		m_pAudioDevice->Play(packet);

		if (newChannel)
			slot.pChannels.push_back(newChannel);

		return;
	}

	// --------------------------------------
	// 단일 채널(키당 1채널)도 "vector front"로 통일
	//  - front만 유지하고 나머지는 정리(안전)
	// --------------------------------------

	if (!continuePlay)
	{
		if (now - slot.lastPlayTime < 0.0017f)
			return;
		slot.lastPlayTime = now;
	}

	// 이미 채널들이 쌓여 있으면 정리(특히 stop/전환 시)
	Update_Audio(slot);

	FMOD::Channel* frontChannel = nullptr;
	if (!slot.pChannels.empty())
		frontChannel = slot.pChannels.front();

	bool isPlaying = false;
	if (frontChannel && frontChannel->isPlaying(&isPlaying) != FMOD_OK)
		isPlaying = false;

	if (continuePlay && frontChannel && isPlaying)
	{
		ClearFadePoints(frontChannel);
		frontChannel->setVolume(slot.fVolume);
		frontChannel->setPaused(false);
		return;
	}

	// front 채널 교체: 기존 front만 stop하고 제거
	if (frontChannel)
	{
		frontChannel->stop();
		slot.pChannels.erase(slot.pChannels.begin());
		frontChannel = nullptr;
	}

	FMOD::Channel* newChannel = nullptr;

	AUDIO_PACKET packet{};
	packet.ppChannelToUpdate = &newChannel;
	packet.pSound = slot.pSound;
	packet.isInfinite = slot.isInfinite;
	packet.isPaused = startPaused;
	packet.is3DAttribute = slot.is3DAttribute;
	packet.fVolume = slot.fVolume;
	packet.iLoopCount = slot.isInfinite ? -1 : slot.iLoopCount;
	packet.eGroup = slot.eGroup;
	packet.vPosition = &m_vPos;

	m_pAudioDevice->Play(packet);

	if (newChannel)
	{
		if (!slot.pChannels.empty())
			slot.pChannels.clear();

		slot.pChannels.push_back(newChannel);
	}

	slot.hasStopScheduled = false;
	slot.stopDspClock = 0;
}

void CAudioSource::Update_Audio(AUDIO_SLOT& slot)
{
	auto& list = slot.pChannels;
	for (size_t idx = 0; idx < list.size(); )
	{
		bool isPlaying = false;
		if (!list[idx] || list[idx]->isPlaying(&isPlaying) != FMOD_OK || !isPlaying)
		{
			list.erase(list.begin() + idx);
			continue;
		}
		++idx;
	}
}

CAudioSource::SequenceBuilder CAudioSource::Sequence(const string& seqKey)
{
	auto& seq = m_Sequences[seqKey];
	return SequenceBuilder(*this, seq);
}

HRESULT CAudioSource::Add_Sequence(const string& seqKey, initializer_list<const char*> slotKeys)
{
	auto& seq = m_Sequences[seqKey];
	seq.slotKeys.clear();
	seq.slotKeys.reserve(slotKeys.size());

	for (const char* raw : slotKeys)
	{
		string k = raw ? raw : "";

		const size_t slashPos = k.find_last_of("/\\");
		const size_t dotPos = k.find_last_of('.');

		const bool hasExt = (dotPos != string::npos) && (slashPos == string::npos || dotPos > slashPos);
		if (!hasExt)
			k += ".wav";

		seq.slotKeys.push_back(move(k));
	}

	if (seq.idx >= seq.slotKeys.size()) seq.idx = 0;
	return S_OK;
}

CAudioSource::AUDIO_SLOT& CAudioSource::SequenceBuilder::PlayNext()
{
	if (ownerSeq.slotKeys.empty())
		return CAudioSource::EmptySlot;

	_float now = CGameInstance::GetInstance()->Get_TimeMgr()->Get_TotalTime("Audio_Timer");
	if (now - ownerSeq.lastPlayTime < 0.05f)
		return CAudioSource::EmptySlot;

	ownerSeq.lastPlayTime = now;

	if (ownerSeq.idx >= ownerSeq.slotKeys.size())
		ownerSeq.idx = 0;

	const string& key = ownerSeq.slotKeys[ownerSeq.idx];
	ownerSeq.idx = (ownerSeq.idx + 1) % (_uint)ownerSeq.slotKeys.size();

	return ownerRef.Slot(key)
		.Infinite(ownerSeq.isInfinite)
		.Loop(ownerSeq.loopCount)
		.Pause(ownerSeq.isPaused)
		.Attribute3D(ownerSeq.is3D)
		.Group(ownerSeq.group)
		.Volume(ownerSeq.volume)
		.Play();
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
	m_Sequences.clear();
}
