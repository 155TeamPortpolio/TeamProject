#include "pch.h"
#include "UI_Seoriyeol.h"
#include "UI_DamageText.h"
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
    dt *= 1.2f;

    __super::Update(dt);

    if (m_followHandle.isValid())
    {
        auto obj = ObjectManager()->Request_Object(m_followHandle);
        Vector3 p = obj->Get_WorldPos();
        p += m_followOffset;
        m_worldPos = _float3(p.x, p.y, p.z);
    }

    Update_WorldToScreen(m_worldPos);

    {
        m_vAnchorOffset.x -= 800.f;
        m_vAnchorOffset.y -= 500.f;
    }

    Get_Component<CObjectContainer>()->UpdateChild(dt);

    if (Is_AnimFinished())
        UI_DeActive({});
}

void CUI_Seoriyeol::UI_Active(void* arg)
{
    auto desc = static_cast<DAMAGE_DESC*>(arg);

    m_followHandle = {};
    m_followOffset = Vector3(0.f, 0.f, 0.f);
    m_worldPos = _float3(0.f, 0.f, 0.f);

    if (desc)
    {
        m_followHandle = desc->followHandle;

        if (m_followHandle.isValid())
        {
            m_followOffset = Vector3(0.f, 1.3f, 0.f);
            if (desc->followOffset.LengthSquared() > 0.f) m_followOffset = desc->followOffset;

            auto obj = ObjectManager()->Request_Object(m_followHandle);
            Vector3 p = obj->Get_WorldPos();
            p += m_followOffset;
            m_worldPos = _float3(p.x, p.y, p.z);
        }
        else
        {
            m_worldPos = _float3(desc->pos.x, desc->pos.y, desc->pos.z);
        }
    }

    Update_WorldToScreen(m_worldPos);

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