#include "pch.h"
#include "ElectricBoo.h"

#include "GameInstance.h"
#include "FieldSystem.h"
#include "ObjectContainer.h"

//component
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

#include "PairBoo.h"

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
	pResource->Add_ResourcePath("Exploreboo.model", "../Bin/Resources/MainCity/NPC/ElectricBoo/ExploreBoo/Exploreboo.model");
	pResource->Add_ResourcePath("Exploreboo.mat", "../Bin/Resources/MainCity/NPC/ElectricBoo/ExploreBoo/Exploreboo.mat");
	pResource->Add_ResourcePath("NPC_Bangboo_Exploreboo_Meta.json", "../Bin/Resources/MainCity/NPC/ElectricBoo/ExploreBoo/NPC_Bangboo_Exploreboo_Meta.json");

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "Exploreboo.model");
	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "Exploreboo.mat");

	pMaterial->SetBlendHasAlpha(AlphaCheckLevel::Precise, "Blend");
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
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Exploreboo.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "NPC_Bangboo_Exploreboo_Meta.json");

	m_strAnimName = "NPC_Bangboo_Exploreboo_Ani_HSC_";
	m_strName = L"전류감별부";

	pAnimator->Set_Animation(Get_AnimName() + "Idle")
		.Loop(true)
		.Apply();

	__super::Awake();

	Add_InteractZone(Get_Position(), _float3(0.f, 0.f, 1.0f), Get_WorldRotation());
	Add_DialoguePartner();
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

void CElectricBoo::Add_DialoguePartner()
{
	auto pObjectContainer = Get_Component<CObjectContainer>();
	PrototypeManager()->Add_ProtoType(G_GlobalLevelKey, "Proto_GameObject_PairBoo", CPairBoo::Create());
	
	CCT_DESC pairBooCCT;
	pairBooCCT.iCollisionMask = 0xFFFFFFFF;
	pairBooCCT.bAutoFit = false;
	pairBooCCT.fHeight = Get_Component<CCharacterController>()->Get_Height();
	pairBooCCT.fRadius = 0.4f;
	pairBooCCT.eGroup = COLLISION_GROUP::COMMON;
	_float4 Pos = Get_Position();
	pairBooCCT.vPos = { Pos.x - 0.6f, Pos.y, Pos.z - 0.4f };

	auto pTransform = Get_Component<CTransform>();
	pTransform->LookAt(Vector4(Pos.x - 0.6f, Pos.y, Pos.z - 0.4f, 1.f));

	CGameObject* pPairBoo = Builder::Create_Object({ G_GlobalLevelKey, "Proto_GameObject_PairBoo" })
		.CharacterController(pairBooCCT)
		.Rotate(Get_WorldRotation() + Vector3(0.f, XM_PI, 0.f))
		.Build("Partner");

	pObjectContainer->Add_Child(pPairBoo, false);
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
