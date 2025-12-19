#pragma once
#include "pch.h"
#include "StateParameter.h"

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
