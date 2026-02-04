#include "pch.h"
#include "UI_Seoriyeol.h"
// Engine
#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_Seoriyeol::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    Add_Component<CObjectContainer>();
    return S_OK;
}

HRESULT CUI_Seoriyeol::Initialize(INIT_DESC* arg)
{
    __super::Initialize(arg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("Seoriyeol.json")));
    Cache_Children();

    return S_OK;
}

void CUI_Seoriyeol::Update(_float dt)
{
    //dt *= 0.1f;

    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);

    if (Is_AnimFinished())
        UI_DeActive({});
}

void CUI_Seoriyeol::UI_Active(void* arg)
{
    SetAllChildAnim(0);
}

void CUI_Seoriyeol::UI_DeActive(void* arg)
{
    UIManager()->Remove_UIObject(this);
}

_bool CUI_Seoriyeol::Is_AnimFinished()
{
    for (auto child : m_children)
        if (!child->Is_AnimFinished())
            return false;

    return true;
}

void CUI_Seoriyeol::Cache_Children()
{
    for (_int i = 0; i < ENUM(CHILD::END); ++i)
        m_children[i] = static_cast<CUI_Object*>(Get_Component<CObjectContainer>()->Find_Descendant(CHILD_INSTNAMES[i]));
}

void CUI_Seoriyeol::SetAllChildAnim(_int idx) const
{
    for (auto child : m_children)
        child->Set_Animation(idx);
}

CUI_Seoriyeol* CUI_Seoriyeol::Create()
{
    auto inst = new CUI_Seoriyeol();
    if (FAILED(inst->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Seoriyeol");
        Safe_Release(inst);
    }
    return inst;
}

CGameObject* CUI_Seoriyeol::Clone(INIT_DESC* pArg)
{
    auto inst = new CUI_Seoriyeol(*this);
    if (FAILED(inst->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Seoriyeol");
        Safe_Release(inst);
    }
    return inst;
}