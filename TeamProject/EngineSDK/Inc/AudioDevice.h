#pragma once
#include "IAudioService.h"

class CAudioDevice final :
	public IAudioService
{
public:

private:
	CAudioDevice();
	virtual ~CAudioDevice() = default;
	HRESULT Initialize();
public:
	virtual void Update() override;
	virtual FMOD::System* Get_System() override;
	//사운드 시스템
	virtual void Play(AUDIO_PACKET& packet)override;
	virtual void Set_Listener(class CTransform* pTransform) override;

public:
	virtual void Set_GroupVolume(SOUND_GROUP group, _float volume);
	virtual void StopAll() override;

private:
	void ApplyLoop(FMOD::Channel* channel, const AUDIO_PACKET& packet);
	void    ClearFadePoints(FMOD::Channel* channel);

private:
	FMOD::System* m_pSystem = { nullptr };
	vector<FMOD::Channel*> m_Channels;
	vector<FMOD::ChannelGroup*> m_Groups;
	class CTransform* m_pTransform = { nullptr };

public:
	static CAudioDevice* Create();
	virtual void Free() override;
};

