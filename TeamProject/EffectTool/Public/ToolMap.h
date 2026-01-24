#pragma once
#include "GameObject.h"

NS_BEGIN(EffectTool)

class CToolMap :
    public CGameObject
{
private:
    CToolMap();
    CToolMap(const CToolMap& rhg);
    virtual ~CToolMap() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    static CToolMap* Create();
    virtual void Free() override;
    CGameObject* Clone(INIT_DESC* pArg) override;

private:

};

NS_END