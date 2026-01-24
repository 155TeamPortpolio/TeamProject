#include "pch.h"
#include "ElectricBoo.h"

#include "GameInstance.h"
#include "FieldSystem.h"

//component
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

CElectricBoo::CElectricBoo()
	:CDialogueNpc()
{
}

CElectricBoo::CElectricBoo(const CElectricBoo& rhs)
	:CDialogueNpc(rhs)
{
}

void CElectricBoo::Execute()
{
	UI_DIALOGUE_REQUEST_DESC desc;
	desc.strDialogueID = m_DiagloueData.StartDialogueID;
	desc.iSequenceID = m_iNextSequceID;
	EventSystem()->Broadcast<UI_DIALOGUE_REQUEST_DESC>({ desc });
}

HRESULT CElectricBoo::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	auto pResource = ResourceManager();
	pResource->Add_ResourcePath("NPC_Woof.model", "../Bin/Resources/MainCity/NPC/Howl/NPC_Woof.model");
	pResource->Add_ResourcePath("NPC_Woof.mat", "../Bin/Resources/MainCity/NPC/Howl/NPC_Woof.mat");
	pResource->Add_ResourcePath("NPC_Special_Woof_Meta.json", "../Bin/Resources/MainCity/NPC/Howl/NPC_Special_Woof_Meta.json");

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "NPC_Woof.model");
	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "NPC_Woof.mat");

	return S_OK;
}

HRESULT CElectricBoo::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CElectricBoo::Awake()
{
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "NPC_Woof.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "NPC_Special_Woof_Meta.json");

	m_strAnimName = "NPC_Special_Woof_Ani_";
	m_strName = L"전류감별부";

	pAnimator->Set_Animation(Get_AnimName() + "Idle01")
		.Loop(true)
		.Apply();

	__super::Awake();

	Add_InteractZone(Get_Position(), _float3(0.f, 0.f, 1.0f), Get_WorldRotation());
}

void CElectricBoo::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CElectricBoo::Update(_float dt)
{
	__super::Update(dt);
}

void CElectricBoo::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

CElectricBoo* CElectricBoo::Create()
{
	CElectricBoo* instance = new CElectricBoo();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CElectricBoo");
	}

	return instance;
}

CGameObject* CElectricBoo::Clone(INIT_DESC* pArg)
{
	CElectricBoo* instance = new CElectricBoo(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CElectricBoo");
	}
	return instance;
}

void CElectricBoo::Free()
{
	__super::Free();
}
