#include "pch.h"
#include "StateMachine.h"
#include "Animator3D.h"
#include "TestObject.h"

template<typename Type>
CStateMachine<Type>::CStateMachine()
{
}

template<typename Type>
CStateMachine<Type>::~CStateMachine()
{
    for (auto& pair : m_States)
        Safe_Delete(pair.second);

    m_States.clear();
    m_Parameters.clear();
    m_Transitions.clear();
    m_AnyStateTransitions.clear();
}

template<typename Type>
HRESULT CStateMachine<Type>::Initialize(Type* pOwner)
{
    if (nullptr == pOwner)
        return E_FAIL;

    m_pOwner = pOwner;

    if (!m_strDefaultState.empty())
        Change_State(m_strDefaultState);

    return S_OK;
}

template<typename Type>
void CStateMachine<Type>::Update(_float dt)
{
    if (nullptr == m_pCurrentState)
        return;

    Update_AnimProgress();

    m_fStateTime += dt;
    m_pCurrentState->m_fStateTime = m_fStateTime;

    Check_AnyStateTransitions();
    Check_Transitions();

    if (m_pCurrentState)
        m_pCurrentState->Update(m_pOwner, dt);
}

template<typename Type>
void CStateMachine<Type>::Update_AnimProgress()
{
    if (!m_pCurrentState || !m_pOwner)
        return;

    CGameObject* pGameObject = static_cast<CGameObject*>(m_pOwner);
    if (pGameObject)
    {
        CAnimator3D* pAnimator = pGameObject->Get_Component<CAnimator3D>();
        if (pAnimator)
        {
            _float fProgress = pAnimator->Get_CurAnimDuration(0);
            m_pCurrentState->m_fAnimProgress = fProgress;
        }
    }
}

template<typename Type>
HRESULT CStateMachine<Type>::Register_State(const string& strState, IBaseState<Type>* pState)
{
    if (nullptr == pState)
        return E_FAIL;

    auto iter = m_States.find(strState);
    if (iter != m_States.end())
    {
        //MSG_BOX("StateMachine::Register_State - State already exists");
        return E_FAIL;
    }

    pState->Set_StateName(strState);
    m_States[strState] = pState;

    return S_OK;
}

template<typename Type>
void CStateMachine<Type>::UnRegister_State(const string& strState)
{
    auto iter = m_States.find(strState);
    if (iter == m_States.end())
        return;

    if (m_pCurrentState == iter->second)
        m_pCurrentState = nullptr;

    Safe_Delete(iter->second);
    m_States.erase(iter);
}

template<typename Type>
IBaseState<Type>* CStateMachine<Type>::Get_State(const string& strState)
{
    auto iter = m_States.find(strState);
    if (iter == m_States.end())
        return nullptr;

    return iter->second;
}

template<typename Type>
HRESULT CStateMachine<Type>::Register_Transition(const TRANSITION_INFO& transition)
{
    if (transition.strFromState.empty() || transition.strToState.empty())
        return E_FAIL;

    m_Transitions.push_back(transition);

    return S_OK;
}

template<typename Type>
HRESULT CStateMachine<Type>::Register_Transition(const string& strFrom, const string& strTo,
    const TRANSITION_CONDITION eCondition, const string& strParam, _float fTimer)
{
    if (strFrom.empty() || strTo.empty())
        return E_FAIL;

    TRANSITION_INFO transition;
    transition.strFromState = strFrom;
    transition.strToState = strTo;
    transition.eCondition = eCondition;
    transition.strParameter = strParam;
    transition.fTimer = fTimer;

    m_Transitions.push_back(transition);

    return S_OK;
}

template<typename Type>
HRESULT CStateMachine<Type>::Register_AnyStateTransition(const string& strTo,
    TRANSITION_CONDITION eCondition, const string& strParam)
{
    if (strTo.empty())
        return E_FAIL;

    TRANSITION_INFO transition;
    transition.strFromState = "AnyState";
    transition.strToState = strTo;
    transition.eCondition = eCondition;
    transition.strParameter = strParam;

    m_AnyStateTransitions.push_back(transition);

    return S_OK;
}

template<typename Type>
void CStateMachine<Type>::Change_State(const string& strState)
{
    auto iter = m_States.find(strState);
    if (iter == m_States.end())
    {
        //MSG_BOX("StateMachine::Change_State - State not found");
        return;
    }

    if (m_pCurrentState && !m_pCurrentState->Handle_Transition(strState))
        return;

    if (m_pCurrentState)
    {
        m_pCurrentState->Begin_Transition(m_pOwner, strState);
        m_pCurrentState->Exit(m_pOwner);
    }

    string strPrevState = m_strCurrentState;

    m_pCurrentState = iter->second;
    m_strCurrentState = strState;
    m_fStateTime = 0.f;
    m_pCurrentState->m_fStateTime = 0.f;
    m_pCurrentState->m_fAnimProgress = 0.f;

    m_pCurrentState->Enter(m_pOwner);
    m_pCurrentState->End_Transition(m_pOwner, strPrevState);

    for (auto& pair : m_Parameters)
        pair.second.Reset_Trigger();
}

template<typename Type>
void CStateMachine<Type>::Set_DefaultState(const string& strState)
{
    m_strDefaultState = strState;
}

template<typename Type>
void CStateMachine<Type>::Check_Transitions()
{
    for (auto& transition : m_Transitions)
    {
        if (transition.strFromState != m_strCurrentState)
            continue;

        if (Check_Transition(transition))
        {
            Change_State(transition.strToState);
            return;
        }
    }
}

template<typename Type>
void CStateMachine<Type>::Check_AnyStateTransitions()
{
    for (auto& transition : m_AnyStateTransitions)
    {
        if (transition.strToState == m_strCurrentState)
            continue;

        if (Check_Transition(transition))
        {
            Change_State(transition.strToState);
            return;
        }
    }
}

template<typename Type>
_bool CStateMachine<Type>::Check_Transition(const TRANSITION_INFO& transition)
{
    if (transition.bExitTime && m_pCurrentState)
    {
        if (m_pCurrentState->Get_AnimProgress() < transition.fExitTime)
            return false;
    }

    switch (transition.eCondition)
    {
    case CONDITION_NONE:
        return true;

    case CONDITION_ANIMATION_END:
        return m_pCurrentState && m_pCurrentState->Is_AnimEnd();

    case CONDITION_TIME_GREATER:
        return m_fStateTime > transition.fTimer;

    case CONDITION_TIME_LESS:
        return m_fStateTime < transition.fTimer;

    case CONDITION_BOOL_TRUE:
        return Get_Bool(transition.strParameter);

    case CONDITION_BOOL_FALSE:
        return !Get_Bool(transition.strParameter);

    case CONDITION_TRIGGER:
        return Get_Trigger(transition.strParameter);
    }

    return false;
}

template<typename Type>
void CStateMachine<Type>::Set_Float(const string& strParam, _float fValue)
{
    m_Parameters[strParam].Set_Float(fValue);
}

template<typename Type>
void CStateMachine<Type>::Set_Int(const string& strParam, _int iValue)
{
    m_Parameters[strParam].Set_Int(iValue);
}

template<typename Type>
void CStateMachine<Type>::Set_Bool(const string& strParam, _bool bValue)
{
    m_Parameters[strParam].Set_Bool(bValue);
}

template<typename Type>
void CStateMachine<Type>::Set_Trigger(const string& strParam)
{
    m_Parameters[strParam].Set_Trigger();
}

template<typename Type>
void CStateMachine<Type>::Reset_Trigger(const string& strParam)
{
    auto iter = m_Parameters.find(strParam);
    if (iter != m_Parameters.end())
        iter->second.Reset_Trigger();
}

template<typename Type>
_float CStateMachine<Type>::Get_Float(const string& strParam) const
{
    auto iter = m_Parameters.find(strParam);
    if (iter == m_Parameters.end())
        return 0.f;

    return iter->second.Get_Float();
}

template<typename Type>
_int CStateMachine<Type>::Get_Int(const string& strParam) const
{
    auto iter = m_Parameters.find(strParam);
    if (iter == m_Parameters.end())
        return 0;

    return iter->second.Get_Int();
}

template<typename Type>
_bool CStateMachine<Type>::Get_Bool(const string& strParam) const
{
    auto iter = m_Parameters.find(strParam);
    if (iter == m_Parameters.end())
        return false;

    return iter->second.Get_Bool();
}

template<typename Type>
_bool CStateMachine<Type>::Get_Trigger(const string& strParam) const
{
    auto iter = m_Parameters.find(strParam);
    if (iter == m_Parameters.end())
        return false;

    return iter->second.Get_Trigger();
}



template class CStateMachine<class CTestObject>;