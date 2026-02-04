#include "pch.h"
#include "UI_Swaebing.h"
// Engine
#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_Swaebing::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    Add_Component<CObjectContainer>();
    return S_OK;
}

HRESULT CUI_Swaebing::Initialize(INIT_DESC* arg)
{
    __super::Initialize(arg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("Swaebing.json")));
    Cache_Children();

    return S_OK;
}

void CUI_Swaebing::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);

    if (Is_AnimFinished())
        UI_DeActive({});
}

void CUI_Swaebing::UI_Active(void* arg)
{
    SetAllChildAnim(0);
}

void CUI_Swaebing::UI_DeActive(void* arg)
{
    UIManager()->Remove_UIObject(this);
}

_bool CUI_Swaebing::Is_AnimFinished()
{
    for (auto child : m_children)
        if (!child->Is_AnimFinished())
            return false;

    return true;
}

void CUI_Swaebing::Cache_Children()
{
    for (_int i = 0; i < ENUM(CHILD::END); ++i)
        m_children[i] = static_cast<CUI_Object*>(Get_Component<CObjectContainer>()->Find_Descendant(CHILD_INSTNAMES[i]));
}

void CUI_Swaebing::SetAllChildAnim(_int idx) const
{
    for (auto child : m_children)
        child->Set_Animation(idx);
}

CUI_Swaebing* CUI_Swaebing::Create()
{
    auto inst = new CUI_Swaebing();
    if (FAILED(inst->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Swaebing");
        Safe_Release(inst);
    }
    return inst;
}

CGameObject* CUI_Swaebing::Clone(INIT_DESC* pArg)
{
    auto inst = new CUI_Swaebing(*this);
    if (FAILED(inst->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Swaebing");
        Safe_Release(inst);
    }
    return inst;
}