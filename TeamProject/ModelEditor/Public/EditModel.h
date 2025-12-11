#pragma once
#include "GameObject.h"

NS_BEGIN(ModelEdit)
class CEditModel :
    public CGameObject
{
private:
    CEditModel();
    CEditModel(const CEditModel& rhs);
    virtual ~CEditModel() DEFAULT;

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
    static CEditModel* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END
