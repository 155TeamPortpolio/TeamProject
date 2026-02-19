#include "Engine_Defines.h"
#include "SoundData.h"
#include "FMOD_Inc/fmod.hpp"
#include "GameInstance.h"
#include "IAudioService.h"

CSoundData::CSoundData(const string& soundKey)
	:m_SoundKey{ soundKey }
{
}

HRESULT CSoundData::Initialize(const string& filePath)
{
    FMOD::System* system = CGameInstance::GetInstance()->Get_AudioDev()->Get_System();
    if (!system) return E_FAIL;

    FMOD_MODE mode = FMOD_DEFAULT | FMOD_3D; // 필요하면 FMOD_2D/CREATESTREAM로 분기
    FMOD_RESULT result = system->createSound(filePath.c_str(), mode, nullptr, &m_pSound);

    return (result == FMOD_OK && m_pSound) ? S_OK : E_FAIL;
}

CSoundData* CSoundData::Create( const string& filePath, const string& soundKey)
{
	CSoundData* instance = new CSoundData(soundKey);
	if (FAILED(instance->Initialize(filePath))) {
		Safe_Release(instance);
		MSG_BOX("Sound Resource Failed to Create : CSoundData");
	}
	return instance;
}

void CSoundData::Free()
{
	if (!m_pSound) 
		return;   // 최소 방어
	
	m_pSound = nullptr;
}
