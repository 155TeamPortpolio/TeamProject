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
    virtual void    Enter(Type* pOwner) override;
    virtual void    Update(Type* pOwner, _float dt) override;
    virtual void    Exit(Type* pOwner) override;

public:
    virtual _bool   Is_EndState() const;

protected:
    CStateMachine<Type>* m_pSubStateMachine = { nullptr };
    _bool                m_bSubInitialized = { false };

public:
    virtual void Free() override;
};

NS_END

#include "StateMachine.h"

NS_BEGIN(Client)

template<typename Type>
void IHState<Type>::Enter(Type* pOwner)
{
    if (m_pSubStateMachine)
    {
        if (!m_bSubInitialized)
        {
            // 루트 상태머신 찾기
            CStateMachine<Type>* pRoot = nullptr;
            _uint iDepth = 1;

            if (this->m_pOwnerStateMachine)
            {
                pRoot = this->m_pOwnerStateMachine->Get_RootStateMachine();
                if (!pRoot)
                    pRoot = this->m_pOwnerStateMachine;
                iDepth = this->m_pOwnerStateMachine->Get_HierarchyDepth() + 1;
            }

            m_pSubStateMachine->Set_RootStateMachine(pRoot, iDepth);

            for (auto& pair : m_pSubStateMachine->Get_States())
            {
                if (pair.second)
                    pair.second->Set_ParentState(this);
            }
            m_pSubStateMachine->Initialize(pOwner);
            m_bSubInitialized = true;
        }
        else
        {
            m_pSubStateMachine->Change_State(m_pSubStateMachine->Get_DefaultStateName());
        }
    }
}

template<typename Type>
void IHState<Type>::Update(Type* pOwner, _float dt)
{
    if (m_pSubStateMachine)
        m_pSubStateMachine->Update(dt);
}

template<typename Type>
void IHState<Type>::Exit(Type* pOwner)
{
    if (m_pSubStateMachine)
    {
        IBaseState<Type>* pSubCurrent = m_pSubStateMachine->Get_CurrentState();
        if (pSubCurrent)
            pSubCurrent->Exit(pOwner);
    }
}

template<typename Type>
_bool IHState<Type>::Is_EndState() const
{
    if (!m_pSubStateMachine)
        return false;

    IBaseState<Type>* pCurrent = m_pSubStateMachine->Get_CurrentState();
    if (!pCurrent)
        return false;

    // 현재 상태가 직접 End 태그를 가지고 있으면
    if (pCurrent->Get_Tag() == "End")
        return true;

    // 현재 상태가 계층구조를 가지고 있으면 재귀 체크
    IHState<Type>* pSubH = dynamic_cast<IHState<Type>*>(pCurrent);
    if (pSubH && pSubH->Has_SubStateMachine())
    {
        IBaseState<Type>* pSubCurrent = pSubH->Get_SubStateMachine()->Get_CurrentState();
        return (pSubCurrent && pSubCurrent->Get_Tag() == "End");
    }

    return false;
}

template<typename Type>
void IHState<Type>::Free()
{
    Safe_Release(m_pSubStateMachine);
    __super::Free();
}

NS_END