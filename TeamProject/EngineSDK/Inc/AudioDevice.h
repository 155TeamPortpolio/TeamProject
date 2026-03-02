#pragma once
#include "IAudioService.h"

class CAudioDevice final :
	public IAudioService
{
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
	virtual void  Push_GroupVolume(SOUND_GROUP group, _float tempVolume01, _float fadeOutSec) override;
	virtual _bool Pop_GroupVolume(SOUND_GROUP group, _float fadeInSec) override;
	virtual void StopAll() override;

private:
	void ApplyLoop(FMOD::Channel* channel, const AUDIO_PACKET& packet);
	//void ClearFadePoints(FMOD::Channel* channel);
	void ClearFadePoints(FMOD::ChannelControl* controlPtr);
	void FadeVolume(FMOD::ChannelControl* controlPtr, _float targetVolume01, _float fadeSeconds);

private:
	FMOD::System* m_pSystem = { nullptr };
	vector<FMOD::Channel*> m_Channels;
	vector<FMOD::ChannelGroup*> m_Groups;
	class CTransform* m_pTransform = { nullptr };
	vector<_float> m_groupVolumeStack[ENUM(SOUND_GROUP::END)];
public:
	static CAudioDevice* Create();
	virtual void Free() override;
};

