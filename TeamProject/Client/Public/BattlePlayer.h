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
    OBJECT_HANDLE GetCurCharacterHandle();

    void SetBattleCharacters(vector<CHARACTER> battleCharacters);

public:
    HRESULT Initialize();
    void Priority_Update(_float dt);
    void Update(_float dt);
    void Late_Update(_float dt);

private:
    HRESULT Initialize_CharacterPrototype();
    CGameObject* CreateBattleCharacter(CHARACTER character);

private:
    unordered_map<string, class CCharacter*>    m_BattleCharacters;
    class CCharacter*                           m_pCurrentCharacter = nullptr;
    vector<OBJECT_HANDLE>                       m_CharacterHandles{};

public:
    static CBattlePlayer* Create();
    virtual void Free() override;
};

NS_END