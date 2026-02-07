#include "pch.h"
#include "UI_GachaTextGroupEngine.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "UI_GachaText.h"

void CUI_GachaTextGroupEngine::Show(GachaGrade eGrade)
{
	string strKey = "GachaStage_UI_EngineB_01";

	switch (eGrade)
	{
	case GachaGrade::S:
		strKey = "GachaStage_UI_EngineS_01";
		break;
	case GachaGrade::A:
		strKey = "GachaStage_UI_EngineA_01";
		break;
	}

	if (m_pText)
		m_pText->Change_Model(strKey);
}

HRESULT CUI_GachaTextGroupEngine::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_GachaTextGroupEngine::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Add_Texts();

	return S_OK;
}

void CUI_GachaTextGroupEngine::Update(_float dt)
{
}

HRESULT CUI_GachaTextGroupEngine::Add_Texts()
{
	PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_UIGachaText", CUI_GachaText::Create());
	
	CGameObject* pObj = Builder::Create_Object({ "Gacha_Level", "Proto_GameObject_UIGachaText" })
		.Rotate(_float3(XMConvertToRadians(-18.f), 0.f, 0.f))
		.Position(_float3(0.f, 0.18f, 1.8f))
		.Build("textEngine");
	
	if (!pObj)
		return E_FAIL;

	Get_Component<CObjectContainer>()->Add_Child(pObj);
	m_pText = dynamic_cast<CUI_GachaText*>(pObj);

	return S_OK;
}

CGameObject* CUI_GachaTextGroupEngine::Create()
{
	CUI_GachaTextGroupEngine* pInstance = new CUI_GachaTextGroupEngine;
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUI_GachaTextGroupEngine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_GachaTextGroupEngine::Clone(INIT_DESC* pArg)
{
	CUI_GachaTextGroupEngine* pInstance = new CUI_GachaTextGroupEngine(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_GachaTextGroupEngine");
		Safe_Release(pInstance);
	}
	return pInstance;
}