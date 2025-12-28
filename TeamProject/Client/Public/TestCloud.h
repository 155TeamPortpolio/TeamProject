#pragma once
#include "GameObject.h"
class CTestCloud :
    public CGameObject
{
private:
    CTestCloud();
    CTestCloud(const CTestCloud& rhs);
    virtual ~CTestCloud() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    static CTestCloud* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};

