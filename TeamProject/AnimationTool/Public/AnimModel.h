#pragma once
#include "GameObject.h"
#include <set>

NS_BEGIN(Engine)
class CGameInstance;
class CAnimator3D;
NS_END

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
    //애니매이션 클립 이름을 주기 위해 만들어짐
    void Set_Panel(class CAnimToolPanel* pAnimToolPanel);

private: //GUI Func
    void GUI_LoadResource(_float fChildHeight);
    void GUI_SetModel(_float fChildHeight);

private: //Model Logic
    void Load_ModelOnce();
    void Load_Resource();
    void Set_Model(string ModelTag, string MaterialTag);
    void Set_Animator();
    void Clear_Model();

    void Render_BonePoint(_vector3 vPos, _float dotSize, ImU32 color);

private:
    CGameInstance* m_pGameInstance = { nullptr };
    class CAnimToolPanel* m_pAnimToolPanel = { nullptr };
    set<string> m_ModelTags;
    set<string> m_MaterialTags;

    //Show Selected Tags
    string m_CurModelTag = { "Select Model" };
    string m_CurMaterialTag = { "Select Material" };

    _float m_fDegree = 0.f;

    Matrix testMat;
    _vector3 testVector;

public:
    static CAnimModel* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END
