#pragma once

#include "GameObject.h"

NS_BEGIN(CameraTool)

class CDemoModel : public CGameObject
{
private:
    CDemoModel() = default;
    CDemoModel(const CDemoModel& rhs) : CGameObject(rhs) {}
    virtual ~CDemoModel() DEFAULT;

public:
    HRESULT Initialize_Prototype()      override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake()                     override;
    void    Priority_Update(_float dt)  override {}
    void    Update(_float dt)           override {}
    void    Late_Update(_float dt)      override {}

public:
    void Render_GUI() override;

public:
    static CDemoModel* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END