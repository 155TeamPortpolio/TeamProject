#pragma once
#include "Base.h"
#include "GUI_Context.h"

NS_BEGIN(Engine)
class ENGINE_DLL CBasePanel abstract:
    public CBase
{
protected:
    CBasePanel(GUI_CONTEXT* pContext);
    virtual ~CBasePanel() =default;

public:
    virtual void Update_Panel(_float dt);
    virtual void Render_GUI() PURE;
    _bool Get_Active() { return m_bActive; };
    void Set_Active(_bool Active) { m_bActive = Active; };

protected:
    GUI_CONTEXT* m_pContext;
    _bool m_bActive = { true };
public:
    virtual void Free() override;
};
NS_END