//BattlePlayer.h
#pragma once
#include "Base.h"

NS_BEGIN(Client)

class CBattlePlayer final :
    public CBase
{
private:
    CBattlePlayer();
    virtual ~CBattlePlayer() DEFAULT;

public:
    void Set_BattleCharacters();

public:
    void Update(_float dt);

private:
    unordered_map<string, class CCharacter*> m_BattlePlayers;
    CCharacter* m_pCurrentPlayer = nullptr;

public:
    static CBattlePlayer* Create();
    virtual void Free() override;
};

NS_END