#include "pch.h"
#include "Sarah.h"

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

CSarah::CSarah()
	:CServiceNpc()
{
}

CSarah::CSarah(const CSarah& rhs)
	:CServiceNpc(rhs)
{
}

HRESULT CSarah::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "NPC_Sarah_Model_WithRootKeeper(Clone)out.model");
	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "NPC_Sarah_Model_WithRootKeeper(Clone)out.mat");

	return S_OK;
}

HRESULT CSarah::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSarah::Awake()
{
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "NPC_Sarah_Model_WithRootKeeper(Clone)out.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "NPC_Female_Sarah_Meta.json");

	m_strAnimName = "NPC_Female_Sarah_Ani_";
	m_strName = L"»ç¶ó";

	pAnimator->Set_Animation(Get_AnimName() + "MainCity_Idle_Loop")
		.Loop(true)
		.Apply();

	//__super::Awake();
	m_DiagloueData = CDataBase::GetInstance()->GetNpcIDData(m_strName);
	//Add_NameIndicator();
	CUI_NameIndicator::INDICATOR_DESC* pDesc = new CUI_NameIndicator::INDICATOR_DESC;
	pDesc->strName = m_strName;
	pDesc->pCCT = Get_Component<CCharacterController>();
	m_pNameIndicator = Builder::Create_UIObject({G_GlobalLevelKey, "Proto_GameObject_NameIndicator"})
		.Add_UIDesc(pDesc)
		.Build("nameIndicator");

	if (!m_pNameIndicator)
		return;

	UIManager()->Add_UIObject(m_pNameIndicator, LevelManager()->Get_NowLevelKey());
}

void CSarah::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CSarah::Update(_float dt)
{
	__super::Update(dt);
}

void CSarah::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

CSarah* CSarah::Create()
{
	CSarah* instance = new CSarah();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CSarah");
	}

	return instance;
}

CGameObject* CSarah::Clone(INIT_DESC* pArg)
{
	CSarah* instance = new CSarah(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CSarah");
	}
	return instance;
}

void CSarah::Free()
{
	__super::Free();
	UIManager()->Remove_UIObject(m_pNameIndicator);
}
