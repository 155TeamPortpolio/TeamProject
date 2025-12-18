#pragma once
#include "GameObject.h"
class CTestMap final : public CGameObject
{
private:
    CTestMap();
    CTestMap(const CTestMap& rhs);
    virtual ~CTestMap() DEFAULT;

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
    static CTestMap* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};

