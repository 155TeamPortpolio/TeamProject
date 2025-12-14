#pragma once
#include "GameObject.h"

NS_BEGIN(EffectTool)
class CToolLight :
    public CGameObject
{
private:
    CToolLight();
    CToolLight(const CToolLight& rhs);
    virtual ~CToolLight() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    static CToolLight* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};
NS_END
