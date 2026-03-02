#include "pch.h"
#include "Cecilia.h"

#include "GameInstance.h"
#include "FieldSystem.h"

//component
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

//state
#include "StateMachine.h"
#include <UI_NameIndicator.h>
#include "DataBase.h"

CCecilia::CCecilia()
	:CServiceNpc()
{
}

CCecilia::CCecilia(const CCecilia& rhs)
	:CServiceNpc(rhs)
{
}

HRESULT CCecilia::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "NPC_Cecilia_Model_WithRootKeeper(Clone)out.model");
	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "NPC_Cecilia_Model_WithRootKeeper(Clone)out.mat");

	return S_OK;
}

HRESULT CCecilia::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCecilia::Awake()
{
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "NPC_Cecilia_Model_WithRootKeeper(Clone)out.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "NPC_Female_Cecilia_Meta.json");

	m_strAnimName = "NPC_Female_Cecilia_Ani_";
	m_strName = L"세실리아";

	pAnimator->Set_Animation(Get_AnimName() + "Galgame_Think_Loop")
		.Loop(true)
		.Apply();

	////__super::Awake();
	//m_DiagloueData = CDataBase::GetInstance()->GetNpcIDData(m_strName);
	////Add_NameIndicator();
	//CUI_NameIndicator::INDICATOR_DESC* pDesc = new CUI_NameIndicator::INDICATOR_DESC;
	//pDesc->strName = m_strName;
	//pDesc->pCCT = Get_Component<CCharacterController>();
	//m_pNameIndicator = Builder::Create_UIObject({G_GlobalLevelKey, "Proto_GameObject_NameIndicator"})
	//	.Add_UIDesc(pDesc)
	//	.Build("nameIndicator");
	//
	//if (!m_pNameIndicator)
	//	return;
	//
	//UIManager()->Add_UIObject(m_pNameIndicator, LevelManager()->Get_NowLevelKey());
}

void CCecilia::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CCecilia::Update(_float dt)
{
	__super::Update(dt);
}

void CCecilia::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

CCecilia* CCecilia::Create()
{
	CCecilia* instance = new CCecilia();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CCecilia");
	}

	return instance;
}

CGameObject* CCecilia::Clone(INIT_DESC* pArg)
{
	CCecilia* instance = new CCecilia(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CCecilia");
	}
	return instance;
}

void CCecilia::Free()
{
	__super::Free();
	//UIManager()->Remove_UIObject(m_pNameIndicator);
}
