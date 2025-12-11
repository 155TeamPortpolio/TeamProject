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
	FMOD::System_Create(&m_pSystem);
	m_pSystem->init(m_iChannelSize, FMOD_INIT_NORMAL, 0);

	for(size_t i =0; i<static_cast<int>(SOUND_GROUP::END);++i ){
		FMOD::ChannelGroup* pGroup;
		m_pSystem->createChannelGroup(to_string(i).c_str(), &pGroup);
		m_Groups.push_back(pGroup);
	}
    float dopplerScale = 1.0f; // 도플러 효과 강도
    float distanceFactor = 15.0f; // 1.0이면 1 유닛 = 1m
    float rolloffScale = 0.7f; // 작을수록 거리 감쇠가 완만해짐

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
}

FMOD::System* CAudioDevice::Get_System()
{
	return m_pSystem;
}

void CAudioDevice::Play(AUDIO_PACKET& packet)
{
    // 이미 채널이 있고, 무한 루프/퍼즈 상황이면 여기서 처리
    if (packet.ppChannelToUpdate && *packet.ppChannelToUpdate)
    {
        FMOD::Channel* pOldChannel = *packet.ppChannelToUpdate;
        bool isPlaying = false;

        if (pOldChannel->isPlaying(&isPlaying) == FMOD_OK)
        {
            // 1) 이미 재생 중 + 무한 루프 + 퍼즈 상태가 아님
            if (isPlaying && packet.isInfinite && !packet.isPaused)
            {
                // 새로 playSound 호출하지 않고, 위치/볼륨만 갱신
                if (packet.is3DAttribute)
                {
                    FMOD_VECTOR pos = { packet.vPosition.x, packet.vPosition.y, packet.vPosition.z };
                    FMOD_VECTOR vel = { 0.f, 0.f, 0.f };
                    pOldChannel->set3DAttributes(&pos, &vel);
                }

                pOldChannel->setVolume(packet.fVolume);
                return; // 여기서 끝
            }

            // 2) 이미 채널이 있는데, 이번에 퍼즈 해제하고 싶다면
            if (!isPlaying && packet.isPaused == false)
            {
                // 일단 다시 재생하거나, 여기서 unpause 같은 처리 가능
                pOldChannel->setPaused(false);
                if (packet.is3DAttribute)
                {
                    FMOD_VECTOR pos = { packet.vPosition.x, packet.vPosition.y, packet.vPosition.z };
                    FMOD_VECTOR vel = { 0.f, 0.f, 0.f };
                    pOldChannel->set3DAttributes(&pos, &vel);
                }
                pOldChannel->setVolume(packet.fVolume);
                return;
            }
        }
    }

    m_pSystem->playSound(
        packet.pSound->Get_SoundData(),
        m_Groups[static_cast<_uint>(packet.eGroup)],
        false,                      // 바로 재생 (pause 상태를 별도로 관리하고 싶으면 true로 두고 나중에 setPaused)
        packet.ppChannelToUpdate
    );

    FMOD::Channel* pNewChannel = *packet.ppChannelToUpdate;

    if (pNewChannel)
    {
        if (packet.is3DAttribute)
        {
            FMOD_VECTOR pos = { packet.vPosition.x, packet.vPosition.y, packet.vPosition.z };
            FMOD_VECTOR vel = { 0.f, 0.f, 0.f };
            pNewChannel->set3DAttributes(&pos, &vel);
        }
        else
        {
            pNewChannel->setMode(FMOD_2D);
        }

        pNewChannel->setVolume(packet.fVolume);

        // 인피니트면 -1, 아니면 횟수 기반
        int loopCount = packet.isInfinite ? -1 : packet.iLoopCount;
        pNewChannel->setLoopCount(loopCount);

        // 패킷에서 퍼즈 요청이 왔다면 바로 멈춰서 시작
        if (packet.isPaused)
            pNewChannel->setPaused(true);
    }
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
