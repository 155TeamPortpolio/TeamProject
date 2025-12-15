#pragma once
#include "GameObject.h"
#include <set>

NS_BEGIN(AnimTool)
class CAnimModel :
    public CGameObject
{
private:
    CAnimModel();
    CAnimModel(const CAnimModel& rhs);
    virtual ~CAnimModel() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    void Render_GUI() override;

private: //GUI Func
    void GUI_LoadResource(_float fChildHeight);
    void GUI_SetModel(_float fChildHeight);

private: //Model Logic
    void Load_Resource();
    void Set_Model(string ModelTag, string MaterialTag);
    void Clear_Model();

private:
    set<string> m_ModelTags;
    set<string> m_MaterialTags;

public:
    static CAnimModel* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END
