#include "pch.h"
#include "UI_GachaTextReveal.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "UI_GachaTextGroupAgent.h"
#include "UI_GachaTextGroupEngine.h"

void CUI_GachaTextReveal::Show(const GACHA_RESULT_DESC& desc)
{
	if (m_pGroups[ENUM(desc.Type)])
		m_pGroups[ENUM(desc.Type)]->Show(desc.Grade);
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
	 
	Add_TextGroups();

	return S_OK;
}

void CUI_GachaTextReveal::Update(_float dt)
{
	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

HRESULT CUI_GachaTextReveal::Add_TextGroups()
{
	if(FAILED(Add_TextGroup("Gacha_Level", "Proto_GameObject_UIGachaTextGroupAgent", GachaType::Agent, CUI_GachaTextGroupAgent::Create())))
		return E_FAIL;

	if (FAILED(Add_TextGroup("Gacha_Level", "Proto_GameObject_UIGachaTextGroupEngine", GachaType::Engine, CUI_GachaTextGroupEngine::Create())))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_GachaTextReveal::Add_TextGroup(const string& strLevelTag, const string& strPrototypeTag, GachaType eType, CGameObject* pProto)
{
	PrototypeManager()->Add_ProtoType(strLevelTag, strPrototypeTag, pProto);

	CGameObject* pObj = Builder::Create_Object({ strLevelTag, strPrototypeTag })
		.Build("textGroup");

	if (!pObj)
		return E_FAIL;

	Get_Component<CObjectContainer>()->Add_Child(pObj);
	m_pGroups[ENUM(eType)] = dynamic_cast<CUI_GachaTextGroup*>(pObj);

	return S_OK;
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