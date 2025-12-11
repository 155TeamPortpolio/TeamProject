#pragma once
#include "Component.h"
NS_BEGIN(Engine)

class ENGINE_DLL CAudioSource :
    public CComponent
{
public:
    typedef struct tagAudioSlot {
        _bool isInfinite = { false };
        _bool isPaused = { false };
        _bool is3DAttribute = {true};

        _float fVolume = { 1.f };
        _int iLoopCount = { 0 };
        _float lastPlayTime = -999.f;

        class CSoundData* pSound = {};
        FMOD::Channel* pChanel = { nullptr };
        SOUND_GROUP eGroup = { SOUND_GROUP::SFX };
    }AUDIO_SLOT;

private:
    CAudioSource();
    CAudioSource(const CAudioSource& rhs);
    virtual ~CAudioSource();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(COMPONENT_DESC* pArg) override;

public:
    HRESULT Add_Slot(const string& levelTag, const string& SoundKey, const string& slotKey = {}, bool isLoop = false, SOUND_GROUP eGroup = { SOUND_GROUP::SFX });
    HRESULT Add_Slot(const string& levelTag, const string& SoundKey, const string& slotKey, bool isLoop , SOUND_GROUP eGroup, _float sound);

    void Set_SlotVolume(const string& slotKey, _float fVolume);
    void Set_SlotLoopCount(const string& slotKey, _int iLoopCount);
    void Set_SlotPuase(const string& slotKey, _bool isPaused);
    void Set_3DAttribute(const string& slotKey, _bool _3DAttribute);
    void FadeOut_Volume(const string& slotKey, _float factor);
    void FadeIn_Volume(const string& slotKey, _float factor,_float dst = 1.f);

    void Play(const string& SoundKey);
    void PlayOnce(const string& SoundKey);
    void RePlay(const string& SoundKey);

public:
    void Render_GUI();
private:
    class IAudioService* m_pAudioDevice = { nullptr };
    class CTransform* m_pTransform = { nullptr };
    unordered_map<string, AUDIO_SLOT> m_Audios;
    _float4 m_vPos = {};
public:
    static CAudioSource* Create();
    virtual CComponent* Clone() override;
    virtual void Free() override;
};
NS_END
