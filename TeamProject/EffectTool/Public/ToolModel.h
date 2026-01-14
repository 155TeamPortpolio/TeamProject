#pragma once
#include "GameObject.h"

NS_BEGIN(EffectTool)
class CToolModel :
    public CGameObject
{
private:
    CToolModel();
    CToolModel(const CToolModel& rhs);
    virtual ~CToolModel() DEFAULT;

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
    static CToolModel* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;


private:
    void LoadModel();

    _bool m_IsLoaded = false;
};
NS_END
