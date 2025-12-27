#pragma once
#include "GameObject.h"
#include "Assimps.h"

NS_BEGIN(ModelEdit)
class CBinaryModel :
    public CGameObject
{
private:
    CBinaryModel();
    CBinaryModel(const CBinaryModel& rhs);
    virtual ~CBinaryModel() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    void Render_GUI() override;

private:
    CTexture* m_pTexture = { nullptr };
public:
    static CBinaryModel* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};
NS_END
