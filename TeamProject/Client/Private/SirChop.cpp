#include "pch.h"
#include "SirChop.h"

#include "GameInstance.h"
#include "FieldSystem.h"

//component
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

//state
#include "StateMachine.h"

CSirChop::CSirChop()
    :CServiceNpc()
{
}

CSirChop::CSirChop(const CSirChop& rhs)
    :CServiceNpc(rhs)
{
}

void CSirChop::Execute()
{
   //UI_DIALOGUE_REQUEST_DESC desc;
   //desc.strDialogueID = m_DiagloueData.StartDialogueID;
   //desc.iSequenceID = m_iNextSequceID;
   //EventSystem()->Broadcast<UI_DIALOGUE_REQUEST_DESC>({ desc });
}

HRESULT CSirChop::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	auto pResource = ResourceManager();
	pResource->Add_ResourcePath("NPC_SirChop_Modelout.model", "../Bin/Resources/MainCity/NPC/Sirchop/NPC_SirChop_Modelout.model");
	pResource->Add_ResourcePath("NPC_SirChop_Modelout.mat", "../Bin/Resources/MainCity/NPC/Sirchop/NPC_SirChop_Modelout.mat");
	pResource->Add_ResourcePath("NPC_Male_SirChop_Meta.json", "../Bin/Resources/MainCity/Sirchop/Howl/NPC_Male_SirChop_Meta.json");

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "NPC_SirChop_Modelout.model");
	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "NPC_SirChop_Modelout.mat");


	return S_OK;
}

HRESULT CSirChop::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSirChop::Awake()
{
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "NPC_SirChop_Modelout.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "NPC_Male_SirChop_Meta.json");

	m_strAnimName = "NPC_Male_SirChop_Ani_";
	m_strName = L"ÃÝÆÛ ´ëÀå";

	pAnimator->Set_Animation(Get_AnimName() + "Idle_01")
		.Loop(true)
		.Apply();

	__super::Awake();

	m_pTransform->Scale(_float3(1.2f, 1.2f, 1.2f));
	Add_InteractZone(Get_Position(), _float3(0.f, 0.f, 1.2f), Get_WorldRotation());
}

void CSirChop::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CSirChop::Update(_float dt)
{
	__super::Update(dt);
}

void CSirChop::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

void CSirChop::Success(_uint curSequenceID)
{
	//FieldSystem()->RequestEnter("Noodle", true);
}

CSirChop* CSirChop::Create()
{
	CSirChop* instance = new CSirChop();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CSirChop");
	}
	return instance;
}

CGameObject* CSirChop::Clone(INIT_DESC* pArg)
{
	CSirChop* instance = new CSirChop(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CSirChop");
	}
	return instance;
}

void CSirChop::Free()
{
	__super::Free();
}
