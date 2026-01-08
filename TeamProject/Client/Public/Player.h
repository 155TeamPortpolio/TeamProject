//Player.h
#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
class CPlayer : public CGameObject
{
private:
    CPlayer() {}
    CPlayer(const CPlayer& rhs);
    virtual ~CPlayer() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;
    virtual void    Render_GUI() override;
private:
    //class CFieldPlayer*   m_pFieldPlayer;
    //class CBattlePlayer*  m_pBattlePlayer;

public:
    static CPlayer* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};
NS_END