#include "pch.h"
#include "DemoUI.h"

CDemoUI::CDemoUI()
{
}

CDemoUI::CDemoUI(const CDemoUI& rhs)
	:CUI_Object(rhs)
{
}

HRESULT CDemoUI::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	return S_OK;
}

HRESULT CDemoUI::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	return S_OK;
}

void CDemoUI::Awake()
{
}

void CDemoUI::Priority_Update(_float dt)
{
}

void CDemoUI::Update(_float dt)
{
}

void CDemoUI::Late_Update(_float dt)
{
}

void CDemoUI::Render_GUI()
{
	__super::Render_GUI();
}

CDemoUI* CDemoUI::Create()
{
	CDemoUI* instance = new CDemoUI();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CDemoUI");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CDemoUI::Clone(INIT_DESC* pArg)
{
	CDemoUI* instance = new CDemoUI(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CDemoUI");
		Safe_Release(instance);
	}

	return instance;
}

void CDemoUI::Free()
{
	__super::Free();
}