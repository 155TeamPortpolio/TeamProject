#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)

class CCharacter;

class CBattlePlayer final : public CBase
{
private:
    struct KeyInput
    {
        _int x = 0;
        _int z = 0;

        void  Reset() { x = 0; z = 0; }
        _bool IsZero() const { return x == 0 && z == 0; }
        _bool operator!=(const KeyInput& other) const
        {
            return x != other.x || z != other.z;
        }
    };

    struct InputState
    {
        KeyInput current;
        KeyInput previous;
        KeyInput lastValid;
        KeyInput currentMove;
        KeyInput previousMove;
        _vector3 direction = {};
        _vector3 prevDirection = {};
        _float   bufferTimer = 0.f;

        void ResetBuffer()
        {
            bufferTimer = 0.f;
            lastValid.Reset();
            previousMove.Reset();
            currentMove.Reset();
        }

        _bool IsMoving() const
        {
            return direction.x != 0.f || direction.z != 0.f;
        }
    };

private:
    CBattlePlayer();
    virtual ~CBattlePlayer() DEFAULT;

public:
    HRESULT Initialize();
    void    Awake();
    void    Priority_Update(_float dt);
    void    Update(_float dt);
    void    Late_Update(_float dt);
    void    Render_GUI();

public: // 캐릭터 관리
    OBJECT_HANDLE   GetCurCharacterHandle();
    void            SetBattleCharacters(vector<CHARACTER> battleCharacters);
    void            Active_Battle();
    void            DeActive_Battle();
    void            QuestStart();
    HRESULT         SwitchCharacter(_int iTargetIndex);
    HRESULT         ClearCharacters();

public: // 타겟팅
    OBJECT_HANDLE   GetTargetHandle() const { return m_TargetHandle; }

public: // 입력 제어
    void            Set_Move(_vector3 vPos, _vector3 vRot);
    void            Lock_Input() { m_bLockInput = true; }
    void            UnLock_Input() { m_bLockInput = false; }

public: // 게이지
    _int            GetParryingCount() const { return m_iParryingCount; }
    void            Add_Gauge(_float fEnergy, _float fDecibel);

public: // 콤보 어택
    void            Request_ComboAttack();
    void            Execute_ComboAttack(_bool bNext);
    void            Cancel_ComboAttack();

public: // 연속 패링
    void            Start_ChainParry();
    void            End_ChainParry();
    _bool           Is_ChainParry() const { return m_bChainParry; }

private: // 입력 처리
    void    Update_Input(_float dt);
    void    Process_Movement(_float dt);
    void    Process_Attack();
    void    Process_SpecialAttack();
    void    Process_Evade();
    void    Process_Switch();
    void    Process_Ultimate();
    void    Process_Energy();
    void    Process_Interact();
    void    Process_ComboSelect(_float dt);
    _bool   Can_Input() const { return !m_bLockInput; }

private: // 캐릭터 전환
    void    NotifyCharacterSwitchIn();
    void    NotifyCharacterSwitchOut();
    _bool   Can_Switch() const;
    _bool   Can_SwitchTo(_uint iIndex) const;
    _int    Find_SwitchIndex(_bool bNext) const;

private: // 타겟팅
    void    Update_Target();

private: // UI 동기화
    void    Update_Status();
    void    Sync_ActionUI();

private: // 초기화
    HRESULT      Initialize_CharacterPrototype();
    CGameObject* CreateBattleCharacter(CHARACTER character);

private:
    _bool           m_bAwaked = false;
    // 입력
    InputState      m_input;
    _bool           m_bLockInput = false;
    // 캐릭터 관리
    vector<CCharacter*>     m_BattleCharacters;
    vector<OBJECT_HANDLE>   m_CharacterHandles;
    CCharacter*             m_pCurrentCharacter = nullptr;
    _uint                   m_iCurrentIndex = 0;
    // 캐릭터 전환
    _float          m_fSwitchCooldown = 0.f;
    _vector4        m_vSwitchPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
    _vector4        m_vSwitchLook = XMVectorSet(0.f, 0.f, 0.f, 0.f);
    _bool           m_bSwitchNext = true;
    _bool           m_bReserveParry = false;
    OBJECT_HANDLE   m_ParryHandle;
    // 타겟팅
    OBJECT_HANDLE   m_TargetHandle;
    _bool           m_bLockOn = false;
    _float          m_fLockOnCooldown = 0.f;
    // 콤보 어택
    _bool           m_bComboSelect = false;
    _float          m_fComboSelectTimer = 0.f;
    // 연속 패링
    _bool           m_bChainParry = false;
    // 게이지
    _uint           m_iParryingCount = 6;

    static constexpr _float KEY_BUFFER_TIME = 0.1f;
    static constexpr _float SWITCH_COOLDOWN = 1.f;
    static constexpr _float LOCKON_COOLDOWN = 0.3f;
    static constexpr _float TARGET_MAXDISTANCE = 10.f;
    static constexpr _float TARGET_BOSS_MAXDISTANCE = 100.f;
    static constexpr _float COMBO_SELECT_DURATION = 2.f;

public:
    static CBattlePlayer* Create();
    virtual void Free() override;
};

NS_END