#pragma once
#include "IBaseState.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

template<typename Type>
class IHState abstract : public IBaseState<Type>
{
public:
    virtual ~IHState() DEFAULT;

public:
    void                 Set_SubStateMachine(CStateMachine<Type>* pSubFSM) { m_pSubStateMachine = pSubFSM; }
    CStateMachine<Type>* Get_SubStateMachine() { return m_pSubStateMachine; }
    _bool                Has_SubStateMachine() const { return m_pSubStateMachine != nullptr; }

public:
    virtual void Enter(Type* pOwner) override
    {
        if (m_pSubStateMachine)
            m_pSubStateMachine->Initialize(pOwner);
    }

    virtual void Update(Type* pOwner, _float dt) override
    {
        if (m_pSubStateMachine)
            m_pSubStateMachine->Update(dt);
    }

    virtual void Exit(Type* pOwner) override {}

protected:
    CStateMachine<Type>* m_pSubStateMachine = { nullptr };

public:
    virtual void Free() override
    {
        Safe_Release(m_pSubStateMachine);
        __super::Free();
    }
};

NS_END