#pragma once
#include "IService.h"
NS_BEGIN(Engine)

class ENGINE_DLL IAudioService abstract :
public IService
{
protected:
	virtual ~IAudioService() DEFAULT;

public:
	virtual void Update() PURE;
	virtual void StopAll() PURE;
	virtual  FMOD::System* Get_System() PURE;
	virtual void Play(AUDIO_PACKET& packet) PURE;
	virtual void Set_Listener(class CTransform* pTransform) PURE;
	virtual void Set_GroupVolume(SOUND_GROUP group, _float volume)PURE;

};

NS_END