#include "pch.h"
#include "StateParameter.h"

CStateParameter::CStateParameter()
    : m_eType(PARAM_BOOL)
    , m_bValue(false)
{
}

CStateParameter::CStateParameter(const CStateParameter& rhs)
    : m_eType(rhs.m_eType)
{
    switch (m_eType)
    {
    case PARAM_FLOAT:
        m_fValue = rhs.m_fValue;
        break;
    case PARAM_INT:
        m_iValue = rhs.m_iValue;
        break;
    case PARAM_BOOL:
        m_bValue = rhs.m_bValue;
        break;
    case PARAM_TRIGGER:
        m_bTrigger = rhs.m_bTrigger;
        break;
    }
}

CStateParameter::CStateParameter(CStateParameter&& rhs) noexcept
    : m_eType(rhs.m_eType)
{
    switch (m_eType)
    {
    case PARAM_FLOAT:
        m_fValue = rhs.m_fValue;
        break;
    case PARAM_INT:
        m_iValue = rhs.m_iValue;
        break;
    case PARAM_BOOL:
        m_bValue = rhs.m_bValue;
        break;
    case PARAM_TRIGGER:
        m_bTrigger = rhs.m_bTrigger;
        break;
    }
}

CStateParameter& CStateParameter::operator=(const CStateParameter& rhs)
{
    if (this != &rhs)
    {
        m_eType = rhs.m_eType;

        switch (m_eType)
        {
        case PARAM_FLOAT:
            m_fValue = rhs.m_fValue;
            break;
        case PARAM_INT:
            m_iValue = rhs.m_iValue;
            break;
        case PARAM_BOOL:
            m_bValue = rhs.m_bValue;
            break;
        case PARAM_TRIGGER:
            m_bTrigger = rhs.m_bTrigger;
            break;
        }
    }
    return *this;
}

CStateParameter& CStateParameter::operator=(CStateParameter&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_eType = rhs.m_eType;

        switch (m_eType)
        {
        case PARAM_FLOAT:
            m_fValue = rhs.m_fValue;
            break;
        case PARAM_INT:
            m_iValue = rhs.m_iValue;
            break;
        case PARAM_BOOL:
            m_bValue = rhs.m_bValue;
            break;
        case PARAM_TRIGGER:
            m_bTrigger = rhs.m_bTrigger;
            break;
        }
    }
    return *this;
}


void CStateParameter::Set_Float(_float fValue)
{
	m_eType = PARAM_FLOAT;
	m_fValue = fValue;
}

void CStateParameter::Set_Int(_int iValue)
{
	m_eType = PARAM_INT;
	m_iValue = iValue;
}

void CStateParameter::Set_Bool(_bool bValue)
{
	m_eType = PARAM_BOOL;
	m_bValue = bValue;
}

void CStateParameter::Set_Trigger()
{
	m_eType = PARAM_TRIGGER;
	m_bTrigger = true;
}

void CStateParameter::Reset_Trigger()
{
	if (m_eType == PARAM_TRIGGER)
		m_bTrigger = false;
}

_float CStateParameter::Get_Float() const
{
	if (m_eType == PARAM_FLOAT)
		return m_fValue;
	return 0.f;
}

_int CStateParameter::Get_Int() const
{
	if (m_eType == PARAM_INT)
		return m_iValue;
	return 0;
}

_bool CStateParameter::Get_Bool() const
{
	if (m_eType == PARAM_BOOL)
		return m_bValue;
	return false;
}

_bool CStateParameter::Get_Trigger() const
{
	if (m_eType == PARAM_TRIGGER)
		return m_bTrigger;
	return false;
}
