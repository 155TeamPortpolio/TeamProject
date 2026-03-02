#include "Engine_Defines.h"
#include "AudioDevice.h"
#include "SoundData.h"
#include "Transform.h"
#include "GameInstance.h"
constexpr int m_iChannelSize = { 512 };

CAudioDevice::CAudioDevice()
{
}

HRESULT CAudioDevice::Initialize()
{
	//FMOD 초기화
    FMOD::ChannelGroup* master = nullptr;
    FMOD::System_Create(&m_pSystem);
	m_pSystem->init(m_iChannelSize, FMOD_INIT_NORMAL, 0);
    m_pSystem->getMasterChannelGroup(&master);

	for(size_t i =0; i<static_cast<int>(SOUND_GROUP::END);++i ){
		FMOD::ChannelGroup* pGroup;
		m_pSystem->createChannelGroup(to_string(i).c_str(), &pGroup);
        //if (master && pGroup)
        //    master->addGroup(pGroup);

		m_Groups.push_back(pGroup);
	}
    float dopplerScale = 1.0f; // 도플러 효과 강도
    float distanceFactor = 5.0f; // 1.0이면 1 유닛 = 1m
    float rolloffScale = 0.8f; // 작을수록 거리 감쇠가 완만해짐

    m_pSystem->set3DSettings(dopplerScale, distanceFactor, rolloffScale);

    CGameInstance::GetInstance()->Get_TimeMgr()->Add_Timer("Audio_Timer");
	return S_OK;
}

void CAudioDevice::Update()
{
	if (m_pSystem)
		m_pSystem->update();

    CGameInstance::GetInstance()->Get_TimeMgr()->Update_Timer("Audio_Timer");
    if (m_pTransform) {
        _float3 pos;
        _float3 look;
        _float3 up;
        XMStoreFloat3(&pos, m_pTransform->Get_Pos());
        XMStoreFloat3(&look, m_pTransform->Dir(STATE::LOOK));
        XMStoreFloat3(&up, m_pTransform->Dir(STATE::UP));

        FMOD_VECTOR listenerPos = { pos.x, pos.y, pos.z };
        FMOD_VECTOR listenerVel = { 0, 0, 0 };
        FMOD_VECTOR listenerForward = { look.x, look.y, look.z };
        FMOD_VECTOR listenerUp = { up.x, up.y, up.z };

        m_pSystem->set3DListenerAttributes(0, &listenerPos, &listenerVel, &listenerForward, &listenerUp);
    }

}

void CAudioDevice::StopAll()
{
    FMOD::ChannelGroup* master = nullptr;
    if (!m_pSystem) return;
    if (m_pSystem->getMasterChannelGroup(&master) == FMOD_OK && master)
        master->stop(); // 하위 그룹/채널 전부 stop
}

FMOD::System* CAudioDevice::Get_System()
{
	return m_pSystem;
}

 void CAudioDevice::ApplyLoop(FMOD::Channel* channel, const AUDIO_PACKET& packet)
{
    if (!channel) return;

    const int loopCount = packet.isInfinite ? -1 : packet.iLoopCount;

    if (loopCount != 0)
    {
        channel->setMode(FMOD_LOOP_NORMAL);
        channel->setLoopCount(loopCount);
    }
    else
    {
        channel->setMode(FMOD_LOOP_OFF);
        channel->setLoopCount(0);
    }
}

 void CAudioDevice::Play(AUDIO_PACKET& packet)
 {
     if (!packet.pSound || !packet.ppChannelToUpdate || !m_pSystem)
         return;

     // 1) 재사용 가능한 채널이면 재사용 (재생 중일 때만)
     if (*packet.ppChannelToUpdate)
     {
         FMOD::Channel* oldChannel = *packet.ppChannelToUpdate;

         bool isPlaying = false;
         if (oldChannel->isPlaying(&isPlaying) == FMOD_OK && isPlaying)
         {
             ClearFadePoints(oldChannel);
             ApplyLoop(oldChannel, packet);

             if (packet.is3DAttribute)
             {
                 oldChannel->setMode(FMOD_3D);
                 FMOD_VECTOR vel = { 0.f, 0.f, 0.f };
                 oldChannel->set3DAttributes(packet.vPosition, &vel);
             }
             else
             {
                 oldChannel->setMode(FMOD_2D);
             }

             oldChannel->setVolume(packet.fVolume);
             oldChannel->setPaused(packet.isPaused);
             return;
         }

         // 끝났으면 새로 만들기
         *packet.ppChannelToUpdate = nullptr;
     }

     m_pSystem->playSound(
         packet.pSound->Get_SoundData(),
         m_Groups[static_cast<_uint>(packet.eGroup)],
         true, // 항상 일단 pause 상태로 생성
         packet.ppChannelToUpdate
     );

     FMOD::Channel* newChannel = *packet.ppChannelToUpdate;
     if (!newChannel) return;

     ClearFadePoints(newChannel);

     if (packet.is3DAttribute)
     {
         newChannel->setMode(FMOD_3D);
         FMOD_VECTOR vel = { 0.f, 0.f, 0.f };
         newChannel->set3DAttributes(packet.vPosition, &vel);
     }
     else
     {
         newChannel->setMode(FMOD_2D);
     }

     newChannel->setVolume(packet.fVolume);
     ApplyLoop(newChannel, packet);

     newChannel->setPaused(packet.isPaused);
 }

 //void CAudioDevice::ClearFadePoints(FMOD::Channel* channel)
 //{
 //    if (!channel || !m_pSystem) return;
 //
 //    unsigned long long channelClock = 0, parentClock = 0;
 //    if (channel->getDSPClock(&channelClock, &parentClock) != FMOD_OK) return;
 //
 //    int sampleRate = 0;
 //    if (m_pSystem->getSoftwareFormat(&sampleRate, 0, 0) != FMOD_OK || sampleRate <= 0) return;
 //
 //    const unsigned long long dspNow = parentClock; 
 //    const unsigned long long dspFar = dspNow + (unsigned long long)(30.0f * (double)sampleRate);
 //    channel->removeFadePoints(dspNow, dspFar);
 //}

 void CAudioDevice::ClearFadePoints(FMOD::ChannelControl* controlPtr)
 {
     if (!controlPtr || !m_pSystem) return;

     unsigned long long dspClock = 0, parentClock = 0;
     if (controlPtr->getDSPClock(&dspClock, &parentClock) != FMOD_OK) return;

     int sampleRate = 0;
     if (m_pSystem->getSoftwareFormat(&sampleRate, 0, 0) != FMOD_OK || sampleRate <= 0) return;

     const unsigned long long dspNow = parentClock;
     const unsigned long long dspFar = dspNow + (unsigned long long)(30.0f * (double)sampleRate);
     controlPtr->removeFadePoints(dspNow, dspFar);
 }

 void CAudioDevice::FadeVolume(FMOD::ChannelControl* controlPtr, 
     _float targetVolume01, _float fadeSeconds)
 {
     if (!controlPtr || !m_pSystem) return;

     targetVolume01 = clamp(targetVolume01, 0.f, 1.f);

     ClearFadePoints(controlPtr);

     if (fadeSeconds <= 0.f)
     {
         controlPtr->setVolume(targetVolume01);
         return;
     }

     float currentVolume = 1.f;
     controlPtr->getVolume(&currentVolume);

     unsigned long long dspClock = 0, parentClock = 0;
     if (controlPtr->getDSPClock(&dspClock, &parentClock) != FMOD_OK) return;

     int sampleRate = 0;
     if (m_pSystem->getSoftwareFormat(&sampleRate, 0, 0) != FMOD_OK || sampleRate <= 0) return;

     const unsigned long long dspNow = parentClock;
     const unsigned long long dspEnd = dspNow + (unsigned long long)((double)fadeSeconds * (double)sampleRate);

     controlPtr->addFadePoint(dspNow, currentVolume);
     controlPtr->addFadePoint(dspEnd, targetVolume01);
 }

 void CAudioDevice::Push_GroupVolume(SOUND_GROUP group, _float tempVolume01, _float fadeOutSec)
 {
     const _uint groupIndex = static_cast<_uint>(group);
     if (groupIndex >= ENUM(SOUND_GROUP::END)) return;

     FMOD::ChannelGroup* groupPtr = m_Groups[groupIndex];
     if (!groupPtr) return;

     float currentVolume = 1.f;
     groupPtr->getVolume(&currentVolume);

     m_groupVolumeStack[groupIndex].push_back((_float)currentVolume);
     FadeVolume(groupPtr, tempVolume01, fadeOutSec);
 }

 _bool CAudioDevice::Pop_GroupVolume(SOUND_GROUP group, _float fadeInSec)
 {
     const _uint groupIndex = static_cast<_uint>(group);
     if (groupIndex >= ENUM(SOUND_GROUP::END)) return false;

     FMOD::ChannelGroup* groupPtr = m_Groups[groupIndex];
     if (!groupPtr) return false;

     auto& stackRef = m_groupVolumeStack[groupIndex];
     if (stackRef.empty()) return false;

     const _float restoreVolume = stackRef.back();
     stackRef.pop_back();

     FadeVolume(groupPtr, restoreVolume, fadeInSec);
     return true;
 }

void CAudioDevice::Set_Listener(CTransform* pTransform)
{
    Safe_Release(m_pTransform);
    m_pTransform = pTransform;
    Safe_AddRef(m_pTransform);
}

void CAudioDevice::Set_GroupVolume(SOUND_GROUP group, _float volume)
{
    m_Groups[static_cast<_uint>(group)]->setVolume(volume);
}

CAudioDevice* CAudioDevice::Create()
{
	CAudioDevice* instance = new CAudioDevice();

	if (FAILED(instance->Initialize())) {
		Safe_Release(instance);
	}

	return instance;
}

void CAudioDevice::Free()
{
	__super::Free();

	if (m_pSystem) {
		m_pSystem->close();
		m_pSystem->release();
		m_pSystem = nullptr;
	}
    Safe_Release(m_pTransform);
}
