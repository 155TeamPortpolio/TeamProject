//BattlePlayer.h
#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)
class CBattlePlayer final :
    public CBase
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
    OBJECT_HANDLE   GetCurCharacterHandle();
    HRESULT         SwitchCharacter(CHARACTER character = CHARACTER::END);
    void            SetBattleCharacters(vector<CHARACTER> battleCharacters);

public:
    HRESULT Initialize();
    void Priority_Update(_float dt);
    void Update(_float dt);
    void Late_Update(_float dt);

private:
    void    Update_Input(_float dt);
    void    Process_Movement(_float dt);
    void    Process_Attack();
    void    Process_SpecialAttack();
    void    Process_Evade();
    void    Process_Switch();
    _bool   Can_Switch() const;
    void    Update_Target();

private:
    HRESULT      Initialize_CharacterPrototype();
    CGameObject* CreateBattleCharacter(CHARACTER character);
    void         RotateCharacterQueue();
    void         NotifyCharacterSwitchIn();
    void         NotifyCharacterSwitchOut();

private:
    queue<std::pair<string, class CCharacter*>>     m_BattleCharacters;
    class CCharacter*                               m_pCurrentCharacter = nullptr;
    vector<OBJECT_HANDLE>                           m_CharacterHandles{};
    OBJECT_HANDLE                                   m_TargetHandle;

    InputState   m_input;
    _uint        m_iParryingCount = 6;
    _bool        m_bIsParrying = { false };
    _float       m_fSwitchCooldown = { 0.f };
    _vector4     m_vSwitchPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
    _vector4     m_vSwitchLook = XMVectorSet(0.f, 0.f, 0.f, 0.f);

    static constexpr _float KEY_BUFFER_TIME = 0.1f;
    static constexpr _float SWITCH_COOLDOWN = 1.f;
    static constexpr _float TARGET_MAXDISTANCE = 10.f;

public:
    static CBattlePlayer* Create();
    virtual void Free() override;
};

NS_END