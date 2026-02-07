#include "pch.h"
#include "UI_GachaTextReveal.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "UI_GachaTextGroupEngine.h"

void CUI_GachaTextReveal::Show(const GACHA_RESULT_DESC& desc)
{
	if (m_Groups[ENUM(GROUP::ENGINE)])
		m_Groups[ENUM(GROUP::ENGINE)]->Show(desc.Grade);
}

HRESULT CUI_GachaTextReveal::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_GachaTextReveal::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_Groups.resize(ENUM(GROUP::END));

	Create_TextGroups();

	return S_OK;
}

void CUI_GachaTextReveal::Update(_float dt)
{
}

void CUI_GachaTextReveal::Create_TextGroups()
{
	PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_UIGachaTextGroupEngine", CUI_GachaTextGroupEngine::Create());
	
	CGameObject* pObj = Builder::Create_Object({ "Gacha_Level", "Proto_GameObject_UIGachaTextGroupEngine" })
		.Build("groupEngine");
	
	Get_Component<CObjectContainer>()->Add_Child(pObj);
	m_Groups[ENUM(GROUP::ENGINE)] = dynamic_cast<CUI_GachaTextGroupEngine*>(pObj);
}

CGameObject* CUI_GachaTextReveal::Create()
{
	CUI_GachaTextReveal* pInstance = new CUI_GachaTextReveal;
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUI_GachaTextReveal");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_GachaTextReveal::Clone(INIT_DESC* pArg)
{
	CUI_GachaTextReveal* pInstance = new CUI_GachaTextReveal(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_GachaTextReveal");
		Safe_Release(pInstance);
	}
	return pInstance;
}