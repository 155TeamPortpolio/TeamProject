#include "pch.h"
#include "SilverAnbi.h"

#include "GameInstance.h"

//component
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"

CSilverAnbi::CSilverAnbi()
    :CServiceNpc()
{
}

CSilverAnbi::CSilverAnbi(const CSilverAnbi& rhs)
    :CServiceNpc(rhs)
{
}

void CSilverAnbi::Execute()
{
    UI_DIALOGUE_REQUEST_DESC desc;
    desc.strDialogueID = m_DiagloueData.StartDialogueID;
    desc.iSequenceID = m_iNextSequceID;
    EventSystem()->Broadcast<UI_DIALOGUE_REQUEST_DESC>({ desc });
}

HRESULT CSilverAnbi::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	auto pResource = ResourceManager();
	pResource->Add_ResourcePath("Avatar_Female_Size02_SilverAnby_Model_WithRootKeeperout.model", "../Bin/Resources/Scott/NPC/silverAnbi/Avatar_Female_Size02_SilverAnby_Model_WithRootKeeperout.model");
	pResource->Add_ResourcePath("Avatar_Female_Size02_SilverAnby_Model_WithRootKeeperout.mat", "../Bin/Resources/Scott/NPC/silverAnbi/Avatar_Female_Size02_SilverAnby_Model_WithRootKeeperout.mat");
	pResource->Add_ResourcePath("Avatar_Female_Size02_SilverAnby_Meta.json", "../Bin/Resources/Scott/NPC/silverAnbi//Avatar_Female_Size02_SilverAnby_Meta.json");

	auto pModel = Get_Component<CSkeletalModel>();
	pModel->Link_Model(G_GlobalLevelKey, "Avatar_Female_Size02_SilverAnby_Model_WithRootKeeperout.model");
	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "Avatar_Female_Size02_SilverAnby_Model_WithRootKeeperout.mat");

	pModel->SetDrawable(7, false);
	pModel->SetDrawable(8, false);
	return S_OK;
}

HRESULT CSilverAnbi::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSilverAnbi::Awake()
{
	auto pAnimator = Get_Component<CAnimator3D>();
	pAnimator->LinkAnimate_Model(G_GlobalLevelKey, "Avatar_Female_Size02_SilverAnby_Model_WithRootKeeperout.model");
	pAnimator->Link_MetaData(G_GlobalLevelKey, "Avatar_Female_Size02_SilverAnby_Meta.json");
	pAnimator->Set_MotionBone(13); //Bip001

	m_strAnimName = "Avatar_Female_Size02_SilverAnby_Ani_";
	m_strName = L"11ȣ";

	pAnimator->Set_Animation(Get_AnimName() + "MainCity_Stand_Idle_02_Loop")
		.Loop(true)
		.Apply();

	__super::Awake();

	Add_InteractZone(Get_Position(), _float3(0.f, 0.f, 1.1f), Get_WorldRotation(), _float3(1.5f, 2.f, 1.5f));
}

void CSilverAnbi::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CSilverAnbi::Update(_float dt)
{
	__super::Update(dt);
}

void CSilverAnbi::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

CSilverAnbi* CSilverAnbi::Create()
{
	CSilverAnbi* instance = new CSilverAnbi();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CSilverAnbi");
	}

	return instance;
}

CGameObject* CSilverAnbi::Clone(INIT_DESC* pArg)
{
	CSilverAnbi* instance = new CSilverAnbi(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CSilverAnbi");
	}
	return instance;
}

void CSilverAnbi::Free()
{
	__super::Free();
}
