#pragma once
#include "FieldCharacter.h"

NS_BEGIN(Client)

class CBelle :
    public CFieldCharacter
{
private:
    CBelle();
    CBelle(const CBelle& rhs);
    virtual ~CBelle() DEFAULT;

public:
    CStateMachine<CBelle>* Get_StateMachine() { return m_pStateMachine; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;
    virtual void    Render_GUI() override;

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();

    void         Update_States();
    void         Process_EndState(const string& strCurrentState);
private:
    CStateMachine<CBelle>* m_pStateMachine = { nullptr };

public:
    static CBelle* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END