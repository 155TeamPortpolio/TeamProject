#pragma once
#include "Character.h"

NS_BEGIN(Client)

class CMiyabi final : public CCharacter
{
private:
    CMiyabi();
    CMiyabi(const CMiyabi& rhs);
    virtual ~CMiyabi() DEFAULT;

public:
    CStateMachine<CMiyabi>* Get_StateMachine() { return m_pStateMachine; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;
    virtual void    Render_GUI() override;
    void            Render_OutLine(ID3D11DeviceContext* pContext, _uint idx);
private:
    virtual void Update_Input(_float dt) override;
    void         Update_States();

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    
private:
    HRESULT Add_OutLineRender();

private:
    CStateMachine<CMiyabi>* m_pStateMachine = { nullptr };

public:
    static CMiyabi* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END