#pragma once
#include "GameObject.h"

NS_BEGIN(MapTool)
class CStaticObject :
    public CGameObject
{
private:
    CStaticObject();
    CStaticObject(const CStaticObject& rhs);
    virtual ~CStaticObject() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    void Render_GUI() override;

public:
    static CStaticObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END
