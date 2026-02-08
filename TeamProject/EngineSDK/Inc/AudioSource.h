#pragma once
#include "Component.h"
NS_BEGIN(Engine)

class ENGINE_DLL CAudioSource :
    public CComponent
{
public:
    typedef struct tagAudioSlot {
        _bool isInfinite = { false };/*무한*/
        _bool isPaused = { false };/*정지 상태*/
        _bool is3DAttribute = { false };/*3D 공간*/

        _float fVolume = { 1.f }; /*기본 볼륨*/
        _int iLoopCount = { 0 };
        _float lastPlayTime = -999.f;

        class CSoundData* pSound = { nullptr };
        FMOD::Channel* pChanel = { nullptr };
        SOUND_GROUP eGroup = { SOUND_GROUP::SFX };
    }AUDIO_SLOT;

    class ENGINE_DLL SlotBuilder
    {
    public:
        SlotBuilder(CAudioSource& Ref,AUDIO_SLOT& Slot): ownerRef(Ref),ownerSlot(Slot){}
        SlotBuilder Infinite(_bool Loop) { ownerSlot.isInfinite = Loop; return *this; };
        SlotBuilder Loop(_int Count) { ownerSlot.iLoopCount = Count;return *this; };
        SlotBuilder Pause(_bool Pause) { ownerSlot.isPaused = Pause; return *this;};
        SlotBuilder Attribute3D(_bool Attribute) { ownerSlot.is3DAttribute = Attribute; return *this;};
        SlotBuilder Group(SOUND_GROUP eGROUP) { ownerSlot.eGroup = eGROUP; return *this;};
        SlotBuilder Volume(_float volume) { ownerSlot.fVolume = volume; return *this;};
        AUDIO_SLOT& Play();

    private:
        CAudioSource& ownerRef;
        AUDIO_SLOT& ownerSlot;
    };

public:
    SlotBuilder Slot(const string& slotKey);

private:

    CAudioSource();
    CAudioSource(const CAudioSource& rhs);
    virtual ~CAudioSource();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(COMPONENT_DESC* pArg) override;

public:
    HRESULT Add_Slot(const string& levelTag,const string& SoundKey);
    HRESULT SoundFolder(const string& levelTag, const string& SoundFolder, const string& extension = { ".wav" });    void Set_SlotVolume(const string& slotKey, _float fVolume);
    void Set_SlotLoopCount(const string& slotKey, _int iLoopCount);
    void Set_SlotPuase(const string& slotKey, _bool isPaused);
    void Set_3DAttribute(const string& slotKey, _bool _3DAttribute);
    void FadeOut_Volume(const string& slotKey, _float factor);
    void FadeIn_Volume(const string& slotKey, _float factor,_float dst = 1.f);

    void Play(const string& SoundKey);
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

public:
    static AUDIO_SLOT EmptySlot;
};
NS_END
