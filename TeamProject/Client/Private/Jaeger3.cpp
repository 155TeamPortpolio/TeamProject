#include "pch.h"
#include "Jaeger3.h"

#include "GameInstance.h"
#include "FieldSystem.h"

//component
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

//state
#include "StateMachine.h"

CJaeger3::CJaeger3()
	:CServiceNpc()
{
}

CJaeger3::CJaeger3(const CJaeger3& rhs)
	:CServiceNpc(rhs)
{
}

void CJaeger3::Execute()
{
	//UI_DIALOGUE_REQUEST_DESC desc;
	//desc.strDialogueID = m_DiagloueData.StartDialogueID;
	//desc.iSequenceID = m_iNextSequceID;
	//EventSystem()->Broadcast<UI_DIALOGUE_REQUEST_DESC>({ desc });
	Success(0);
}

HRESULT CJaeger3::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	auto pResource = ResourceManager();
	pResource->Add_ResourcePath("StrikeJaeger.model", "../Bin/Resources/Scott/Npc/Jaeger/StrikeJaeger.model");
	pResource->Add_ResourcePath("StrikeJaeger.mat", "../Bin/Resources/Scott/Npc/Jaeger/StrikeJaeger.mat");
	pResource->Add_ResourcePath("LightJaeger_Meta.json", "../Bin/Resources/Scott/Npc/Jaeger/LightJaeger_Meta.json");

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model("Scott_Level", "StrikeJaeger.model");
	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material("Scott_Level", "StrikeJaeger.mat");

	return S_OK;
}

HRESULT CJaeger3::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CJaeger3::Awake()
{
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model("Scott_Level", "StrikeJaeger.model");
	pAnimator->Link_MetaData("Scott_Level", "LightJaeger_Meta.json");

	m_strAnimName = "LightJaeger_Ani_";
	m_strName = L"경비병";

	pAnimator->Set_Animation(Get_AnimName() + "Maincity_Idle_Normal02")
		.Loop(true)
		.Apply();

	__super::Awake();

	Add_InteractZone(Get_Position(), _float3(0.f, 0.f, 1.2f), Get_WorldRotation());
}

void CJaeger3::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CJaeger3::Update(_float dt)
{
	__super::Update(dt);
}

void CJaeger3::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

void CJaeger3::Success(_uint curSequenceID)
{
	LevelManager()->Request_ChangeLevel("Tutorial_Level", true);
}

CJaeger3* CJaeger3::Create()
{
	CJaeger3* instance = new CJaeger3();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CJaeger3");
	}

	return instance;
}

CGameObject* CJaeger3::Clone(INIT_DESC* pArg)
{
	CJaeger3* instance = new CJaeger3(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CJaeger3");
	}
	return instance;
}

void CJaeger3::Free()
{
	__super::Free();
}
