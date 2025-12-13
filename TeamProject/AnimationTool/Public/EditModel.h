#pragma once
#include "GameObject.h"

NS_BEGIN(AnimTool)
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

private:
    void Load_Resource();
    void Set_Model();
    void Set_Material();
    void Clear_Model();
    void Clear_Material();

private:
    vector<string> m_ModelTag;
    vector<string> m_MaterialTag;

public:
    static CEditModel* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END
