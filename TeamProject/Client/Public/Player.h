//Player.h
#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
class CPlayer : public CGameObject
{
public:
    enum class PLAYER { BATTLE, FIELD, END };

private:
    CPlayer() {}
    CPlayer(const CPlayer& rhs);
    virtual ~CPlayer() DEFAULT;

public:
    void    Set_PlayerType(PLAYER ePlayer);
    PLAYER  Get_PlayerType() const { return m_ePlayerType; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;
    virtual void    Render_GUI() override;

private:
    void Update_KeyInput();

private:
    class CFieldPlayer*   m_pFieldPlayer = nullptr;
    class CBattlePlayer*  m_pBattlePlayer = nullptr;

private:
    PLAYER                m_ePlayerType = PLAYER::BATTLE;

public:
    static CPlayer* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};
NS_END