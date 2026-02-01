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
	return S_OK;
}

void CUI_Gangta::Update(_float dt)
{
    __super::Update(dt);
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
