#pragma once
#include "GameObject.h"
NS_BEGIN(ModelEdit)
class CLightObject :
    public CGameObject
{
private:
    CLightObject();
    CLightObject(const CLightObject& rhs);
    virtual ~CLightObject()DEFAULT;
public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    void Render_GUI() override;

public:
    static CLightObject* Create();
    virtual void Free() override;
    CGameObject* Clone(INIT_DESC* pArg) override;
};

NS_END