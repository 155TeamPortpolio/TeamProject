//BattlePlayer.h
#pragma once
#include "Base.h"

NS_BEGIN(Client)

class CBattlePlayer final :
    public CBase
{
public:
    enum class PLAYER { JANEDOE, CORIN, END };

private:
    CBattlePlayer();
    virtual ~CBattlePlayer() DEFAULT;

public:
    void SetBattleCharacters(vector<_uint> battleCharacters);

public:
    HRESULT Initialize();
    void Priority_Update(_float dt);
    void Update(_float dt);
    void Late_Update(_float dt);

private:
    unordered_map<string, class CCharacter*> m_BattleCharacters;
    CCharacter* m_pCurrentCharacter = nullptr;

public:
    static CBattlePlayer* Create();
    virtual void Free() override;
};

NS_END