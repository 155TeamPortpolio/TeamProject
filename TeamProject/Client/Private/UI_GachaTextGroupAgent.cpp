#include "pch.h"
#include "UI_GachaTextGroupAgent.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "UI_GachaText.h"

void CUI_GachaTextGroupAgent::Show(GachaGrade eGrade)
{
	string strKey = "GachaStage_UI_AgentText_01";

	switch (eGrade)
	{
	case GachaGrade::S:
		strKey = "GachaStage_UI_AgentText_01";
		break;
	case GachaGrade::A:
		strKey = "GachaStage_UI_AgentText_02";
		break;
	case GachaGrade::B:
		strKey = "GachaStage_UI_AgentText_03";
		break;
	}
}

HRESULT CUI_GachaTextGroupAgent::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_GachaTextGroupAgent::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Add_Texts();

	return S_OK;
}

void CUI_GachaTextGroupAgent::Update(_float dt)
{
}

HRESULT CUI_GachaTextGroupAgent::Add_Texts()
{
	PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_UIGachaText", CUI_GachaText::Create());

	if (FAILED(Add_Text(TYPE::LT, "GachaStage_UI_AgentText_02", _float3(0.f, XMConvertToRadians(-45.f), 0.f), _float3(0.8f, 1.32f, 0.f))))
		return E_FAIL;

	if (FAILED(Add_Text(TYPE::LC, "GachaStage_UI_AgentText_01", _float3(0.f, XMConvertToRadians(-45.f), 0.f), _float3(0.8f, 1.f, 0.f))))
		return E_FAIL;

	if (FAILED(Add_Text(TYPE::LB, "GachaStage_UI_AgentText_05", _float3(0.f, XMConvertToRadians(-45.f), 0.f), _float3(0.8f, 0.78f, 0.f))))
		return E_FAIL;

	if (FAILED(Add_Text(TYPE::RT, "GachaStage_UI_ContractText_01", _float3(0.f, XMConvertToRadians(45.f), 0.f), _float3(-0.8f, 1.f, 0.f))))
		return E_FAIL;

	if (FAILED(Add_Text(TYPE::RB, "GachaStage_UI_ContractText_02", _float3(0.f, XMConvertToRadians(45.f), 0.f), _float3(-0.8f, 0.78f, 0.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_GachaTextGroupAgent::Add_Text(TYPE eType, const string& strModelKey, const _float3& vRotation, const _float3& vPosition)
{
	CGameObject* pObj = Builder::Create_Object({ "Gacha_Level", "Proto_GameObject_UIGachaText" })
		.Scale(_float3(1.1f, 1.1f, 1.1f))
		.Rotate(vRotation)
		.Position(vPosition)
		.Build("textAgent");

	if (!pObj)
		return E_FAIL;

	Get_Component<CObjectContainer>()->Add_Child(pObj);

	m_pText[ENUM(eType)] = dynamic_cast<CUI_GachaText*>(pObj);
	m_pText[ENUM(eType)]->Change_Model(strModelKey);

	return S_OK;
}

void CUI_GachaTextGroupAgent::Create_Texts()
{
	PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_UIGachaText", CUI_GachaText::Create());

	CGameObject* pObj = Builder::Create_Object({ "Gacha_Level", "Proto_GameObject_UIGachaText" })
		.Rotate(_float3(XMConvertToRadians(0.f), XMConvertToRadians(-45.f), 0.f))
		.Position(_float3(0.7f, 1.f, 0.f))
		.Build("uiGachaText");

	Get_Component<CObjectContainer>()->Add_Child(pObj);
	m_pText[ENUM(TYPE::LT)] = dynamic_cast<CUI_GachaText*>(pObj);
	m_pText[ENUM(TYPE::LT)]->Change_Model("GachaStage_UI_AgentText_01");
}

CGameObject* CUI_GachaTextGroupAgent::Create()
{
	CUI_GachaTextGroupAgent* pInstance = new CUI_GachaTextGroupAgent;
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUI_GachaTextGroupAgent");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_GachaTextGroupAgent::Clone(INIT_DESC* pArg)
{
	CUI_GachaTextGroupAgent* pInstance = new CUI_GachaTextGroupAgent(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_GachaTextGroupAgent");
		Safe_Release(pInstance);
	}
	return pInstance;
}