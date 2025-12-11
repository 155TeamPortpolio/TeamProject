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
	FMOD::System* pSystem = CGameInstance::GetInstance()->Get_AudioDev()->Get_System();

	FMOD_RESULT fr =pSystem->createSound(filePath.c_str(), FMOD_3D, nullptr, &m_pSound);

	if (fr == FMOD_OK)
		return S_OK;
	else
		return E_FAIL;
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
	m_pSound->release();
}
