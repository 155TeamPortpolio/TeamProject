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

private:
    HRESULT Initialize_CharacterPrototype();
    CGameObject* CreateBattleCharacter(CHARACTER character);

    void RotateCharacterQueue();
    void NotifyCharacterSwitchIn();
    void NotifyCharacterSwitchOut();

private:
    queue<std::pair<string, class CCharacter*>>     m_BattleCharacters;
    class CCharacter*                               m_pCurrentCharacter = nullptr;
    vector<OBJECT_HANDLE>                           m_CharacterHandles{};

private:
    _uint   m_iParryingCount = 6;
    _bool   m_bIsParrying = { false };
    _bool   m_bIsSwitching = { false };

    static constexpr _float SWITCH_COOLDOWN = 1.f;

public:
    static CBattlePlayer* Create();
    virtual void Free() override;
};

NS_END