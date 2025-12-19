#pragma once
#include "Base.h"

NS_BEGIN(Client)

enum PARAMETER_TYPE
{
    PARAM_FLOAT,
    PARAM_INT,
    PARAM_BOOL,
    PARAM_TRIGGER,
};

class CStateParameter final : public CBase
{
private:
    CStateParameter() DEFAULT;
    virtual ~CStateParameter() DEFAULT;

public:
    void    Set_Float(_float fValue);
    void    Set_Int(_int iValue);
    void    Set_Bool(_bool bValue);
    void    Set_Trigger();
    void    Reset_Trigger();

    _float  Get_Float() const;
    _int    Get_Int() const;
    _bool   Get_Bool() const;
    _bool   Get_Trigger() const;

private:
    PARAMETER_TYPE m_eType = PARAM_BOOL;
    union
    {
        _float m_fValue;
        _int   m_iValue;
        _bool  m_bValue;
        _bool  m_bTrigger;
    };
};

NS_END