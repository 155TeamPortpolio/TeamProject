#pragma once
#include "Component.h"
NS_BEGIN(Engine)
class CTransform; class IAudioService; class CSoundData;

class ENGINE_DLL CAudioSource : public CComponent
{
public:
    typedef struct tagAudioSlot 
    {
        string Key = {};
        _bool             isInfinite    = false;/*무한*/
        _bool             isPaused      = false;/*정지 상태*/
        _bool             is3DAttribute = false;/*3D 공간*/
                          
        _float            fVolume       = 1.f; /*기본 볼륨*/
        _int              iLoopCount    = 0;
        _float            lastPlayTime  = -999.f;

        _bool             hasPendingFadeIn = false;
        _float            pendingFadeInSec = 0.f;
        _float            pendingFadeInDst = 1.f;
        _bool             hasStopScheduled = false;
        unsigned long long stopDspClock = 0;
        CSoundData*       pSound{};
        vector<FMOD::Channel*>   pChannels{};
        SOUND_GROUP       eGroup = SOUND_GROUP::SFX;
    }AUDIO_SLOT;

    // Jehyun
    typedef struct tagAudioSequence
    {
        vector<string> slotKeys;
        _uint          idx          = 0;
        _float         lastPlayTime = -999.f;
        _bool          isInfinite   = false;
        _bool          isPaused     = false;
        _bool          is3D         = false;
        _float         volume       = 1.f;
        _int           loopCount    = 0;
        SOUND_GROUP    group        = SOUND_GROUP::SFX;
    }AUDIO_SEQUENCE;

    class ENGINE_DLL SlotBuilder
    {
    public:
        SlotBuilder(CAudioSource& Ref,AUDIO_SLOT& Slot): ownerRef(Ref), ownerSlot(Slot) {}
        SlotBuilder Infinite(_bool Loop)         { ownerSlot.isInfinite    = Loop;      return *this; };
        SlotBuilder Loop(_int Count)             { ownerSlot.iLoopCount    = Count;     return *this; };
        SlotBuilder Attribute3D(_bool Attribute) { ownerSlot.is3DAttribute = Attribute; return *this; };
        SlotBuilder Group(SOUND_GROUP eGROUP)    { ownerSlot.eGroup        = eGROUP;    return *this; };
        SlotBuilder Pause(_bool Pause)           { ownerRef.Set_SlotPuase(ownerSlot.Key, Pause);     return *this; };
        void Stop()                              { ownerRef.Set_SlotStop(ownerSlot.Key); };
        SlotBuilder Volume(_float volume)        { ownerRef.Set_SlotVolume(ownerSlot.Key, volume); return *this; };
        void FadeOut(_float Durationfactor)      { ownerRef.FadeOut_Volume(ownerSlot.Key, Durationfactor); };
        SlotBuilder FadeIn(_float Durationfactor,_float dst = 1.f)        
        {
           return *this; 
        };

        AUDIO_SLOT& Play();
        AUDIO_SLOT& PlayUnique();

    private:
        CAudioSource& ownerRef;
        AUDIO_SLOT& ownerSlot;
    };
    
    // Jehyun
    class ENGINE_DLL SequenceBuilder
    {
    public:
        SequenceBuilder(CAudioSource& Ref, AUDIO_SEQUENCE& Seq) : ownerRef(Ref), ownerSeq(Seq) {}
        SequenceBuilder Infinite(_bool Loop)         { ownerSeq.isInfinite = Loop;   return *this; }
        SequenceBuilder Loop(_int Count)             { ownerSeq.loopCount  = Count;  return *this; }
        SequenceBuilder Pause(_bool Pause)           { ownerSeq.isPaused   = Pause;  return *this; }
        SequenceBuilder Attribute3D(_bool is3D)      { ownerSeq.is3D       = is3D;   return *this; }
        SequenceBuilder Group(SOUND_GROUP eGROUP)    { ownerSeq.group      = eGROUP; return *this; }
        SequenceBuilder Volume(_float volume)        { ownerSeq.volume     = volume; return *this; }
        SequenceBuilder Reset(_uint idx = 0)         { ownerSeq.idx        = idx;    return *this; }
        AUDIO_SLOT&     PlayNext();

    private:
        CAudioSource&   ownerRef;
        AUDIO_SEQUENCE& ownerSeq;
    };

public:
    SlotBuilder     Slot(const string& slotKey);
   

    SequenceBuilder Sequence(const string& seqKey);
    HRESULT         Add_Sequence(const string& seqKey, initializer_list<const char*> slotKeys);
   
    template<typename... Args>
    HRESULT Add_Sequence(const string& seqKey, const char* first, const Args...rest)
    {
        return Add_Sequence(seqKey, {first, rest...});
    }

private:
    CAudioSource();
    CAudioSource(const CAudioSource& rhs);
    virtual ~CAudioSource();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(COMPONENT_DESC* pArg) override;

public:
    HRESULT Add_Slot(const string& levelTag,const string& SoundKey);
    HRESULT SoundFolder(const string& levelTag, const string& SoundFolder, const string& extension = { ".wav" });   
    void    Set_SlotVolume(const string& slotKey, _float fVolume);
    void    Set_SlotLoopCount(const string& slotKey, _int iLoopCount);
    void    Set_SlotPuase(const string& slotKey, _bool isPaused);
    void    Set_SlotStop(const string& slotKey);
    void    Set_3DAttribute(const string& slotKey, _bool _3DAttribute);
    void    FadeOut_Volume(const string& slotKey, _float Durationfactor);
    void    FadeIn_Volume(const string& slotKey, _float Durationfactor,_float dst = 1.f);
    void    Set_AudioPos(_vector3 pos, _vector3 velocity = {}); /*오디오 상에 보낼 3D상 위치*/
    void    Play(const string& soundKey, _bool continuePlay = false, _bool startPaused = false);
    void    PlayUnique(const string& soundKey);
    void    Update_Audio(AUDIO_SLOT& slot);
    void    FadeOutAll(_float Durationfactor);
public:
    void    Render_GUI();
private:
    void    ClearFadePoints(FMOD::Channel* channel);

private:
    IAudioService*                        m_pAudioDevice{};
    FMOD_VECTOR                           m_vPos{};
    FMOD_VECTOR                           m_vVelocity{};
    unordered_map<string, AUDIO_SLOT>     m_Audios;
    unordered_map<string, AUDIO_SEQUENCE> m_Sequences;
public:
    static CAudioSource* Create();
    virtual CComponent* Clone() override;
    virtual void Free() override;

public:
    inline static AUDIO_SLOT EmptySlot{};
};
NS_END
