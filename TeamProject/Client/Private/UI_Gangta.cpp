#include "pch.h"
#include "UI_Gangta.h"
// Engine
#include "ObjectContainer.h"
#include "GameInstance.h"

HRESULT CUI_Gangta::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CObjectContainer>();
	return S_OK;
}

HRESULT CUI_Gangta::Initialize(INIT_DESC* arg)
{
	__super::Initialize(arg);
    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("Gangta.json")));

    Cache_Children();

	return S_OK;
}

void CUI_Gangta::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);

    if (Is_AnimFinished())
        UI_DeActive({});
}

void CUI_Gangta::UI_Active(void* arg)
{
    Set_ChildAnim(CHILD::GANG_IN,         0);
    Set_ChildAnim(CHILD::GANG_OUT,        0);
    Set_ChildAnim(CHILD::TA_IN,           0);
    Set_ChildAnim(CHILD::TA_OUT,          0);
    Set_ChildAnim(CHILD::GANG_OUTLINE,    0);
    Set_ChildAnim(CHILD::TA_OUTLINE,      0);
    Set_ChildAnim(CHILD::FACTORY_IN_01,    0);
    Set_ChildAnim(CHILD::FACTORY_IN_02,    0);
    Set_ChildAnim(CHILD::FACTORY_OUT_01,   0);
    Set_ChildAnim(CHILD::FACTORY_OUT_02,   0);
}

void CUI_Gangta::UI_DeActive(void* arg)
{
    UIManager()->Remove_UIObject(this);
}

_bool CUI_Gangta::Is_AnimFinished()
{
    _bool anim1  = Is_ChildAnimFinished(CHILD::GANG_OUT);
    _bool anim2  = Is_ChildAnimFinished(CHILD::GANG_IN);
    _bool anim3  = Is_ChildAnimFinished(CHILD::TA_OUT);
    _bool anim4  = Is_ChildAnimFinished(CHILD::TA_IN);
    _bool anim5  = Is_ChildAnimFinished(CHILD::GANG_OUTLINE);
    _bool anim6  = Is_ChildAnimFinished(CHILD::TA_OUTLINE);
    _bool anim7  = Is_ChildAnimFinished(CHILD::FACTORY_IN_01);
    _bool anim8  = Is_ChildAnimFinished(CHILD::FACTORY_IN_02);
    _bool anim9  = Is_ChildAnimFinished(CHILD::FACTORY_OUT_01);
    _bool anim10 = Is_ChildAnimFinished(CHILD::FACTORY_OUT_02);

    return anim1 && anim2 && anim3 && anim4 && anim5 && anim6 && anim7 && anim8 && anim9 && anim10;
}

void CUI_Gangta::Cache_Children()
{
    auto container = Get_Component<CObjectContainer>();

    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        const string& instName = CHILD_INSTNAMES[i];
        if (instName.empty())
            continue;

        auto obj = container->Find_Descendant(instName);
        if (!obj)
            continue;

        m_children[i] = static_cast<CUI_Object*>(obj);
    }
}

void CUI_Gangta::Set_ChildAnim(CHILD eChild, _int idx) const
{
    auto child = m_children[ENUM(eChild)];
    if (!child)
        return;
    child->Set_Animation(idx);
}

_bool CUI_Gangta::Is_ChildAnimFinished(CHILD eChild) const
{
    auto child = m_children[ENUM(eChild)];
    if (!child)
        return false;
    return child->Is_AnimFinished();
}

CUI_Gangta* CUI_Gangta::Create()
{
    auto inst = new CUI_Gangta();
    if (FAILED(inst->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Gangta");
        Safe_Release(inst);
    }
    return inst;
}

CGameObject* CUI_Gangta::Clone(INIT_DESC* pArg)
{
    auto inst = new CUI_Gangta(*this);
    if (FAILED(inst->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Gangta");
        Safe_Release(inst);
    }
    return inst;
}
