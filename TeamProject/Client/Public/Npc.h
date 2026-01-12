#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
class CNpc abstract:
    public CGameObject
{
protected:
    CNpc();
    CNpc(const CNpc& rhs);
    virtual ~CNpc() DEFAULT;

public:
    const string&  Get_AnimName() const { return m_strAnimName; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Priority_Update(_float dt) override {};
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override {};
    
protected:
    string m_strAnimName = "";
    string m_strName = "";

public:
    virtual CGameObject* Clone(INIT_DESC* pArg) PURE;
    virtual void Free() override;
};
NS_END