#pragma once
#include "GameObject.h"

NS_BEGIN(Demo)
class CDemoModel :
    public CGameObject
{
private:
    CDemoModel();
    CDemoModel(const CDemoModel& rhs);
    virtual ~CDemoModel() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

    virtual void OnCollisionEnter() override;
    virtual void OnCollisionStay() override;
    virtual void OnCollisionExit() override;
public:
    void Render_GUI() override;

public:
    static CDemoModel* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();

};
NS_END
