#include "pch.h"
#include "UI_AnomalyBase.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_AnomalyBase::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_AnomalyBase::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("test.json")));

	Set_AnchorOffset({200.f, 200.f});

	return S_OK;
}

void CUI_AnomalyBase::Awake()
{
}

void CUI_AnomalyBase::Update(_float dt)
{
	__super::Update(dt);

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}