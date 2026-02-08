#include "pch.h"
#include "UI_GachaTextGroupEngine.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "UI_GachaText.h"

void CUI_GachaTextGroupEngine::Show(GachaGrade eGrade)
{
	__super::Show(eGrade);

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

	if (auto& pText = m_pTexts[ENUM(TYPE::CENTER)])
		pText->Play(strKey);
}

HRESULT CUI_GachaTextGroupEngine::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

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
	__super::Update(dt);
}

HRESULT CUI_GachaTextGroupEngine::Add_Texts()
{
	m_pTexts.resize(ENUM(TYPE::END));

	PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_UIGachaText", CUI_GachaText::Create());
	
	CUI_GachaText::TEXT_DESC* pDesc = new CUI_GachaText::TEXT_DESC;
	pDesc->vScaleOffset = { -0.4f, -0.4f, -0.4f };
	pDesc->vPosOffset = { 0.f, -0.1f, -0.2f }; 

	CGameObject* pObj = Builder::Create_Object({ "Gacha_Level", "Proto_GameObject_UIGachaText" })
		.Add_ObjDesc(pDesc)
		.Rotate(_float3(XMConvertToRadians(-18.f), 0.f, 0.f))
		.Position(_float3(0.f, 0.182f, 1.86f))
		.Build("textEngine");
	
	if (!pObj)
		return E_FAIL;

	Get_Component<CObjectContainer>()->Add_Child(pObj);
	m_pTexts[ENUM(TYPE::CENTER)] = dynamic_cast<CUI_GachaText*>(pObj);

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