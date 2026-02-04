#include "pch.h"
#include "GachaStage.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Child.h"
#include "StaticModel.h"
#include "Material.h"
#include "Light.h"

#include "GachaStageScreen.h"
#include "GachaWeapon.h"
#include "GachaAvatar.h"

#include "CamDirector.h"
#include "DataBase.h"

CGachaStage::CGachaStage()
    :CGameObject()
{
}

CGachaStage::CGachaStage(const CGachaStage& rhs)
    :CGameObject(rhs), m_pResultDesc(rhs.m_pResultDesc)
{
}

void CGachaStage::PlayStageSpin(_int index)
{
	if (m_iSpinIndex == index) return;
	m_iSpinIndex = index;
	SetLightOff();
	Update_StageEnviroment(m_iSpinIndex);
}

void CGachaStage::PlayRevealEffect()
{
	CameraManager()->SetZoomType(ENUM(CamZoomType::GachaShake), 1.8f);
	CameraManager()->AddShakeAxisWave(CamShakeAxis::Roll, 3.f, 4.0f, 0.8f, 0.1f, EaseType::InQuad, EaseType::InOutQuad);
	CameraManager()->AddShakeAxisWave(CamShakeAxis::Yaw, 1.4f, 3.0f, 0.6f, 0.1f, EaseType::InQuad, EaseType::InOutQuad);
	CameraManager()->AddShakeAxisWave(CamShakeAxis::Pitch, 1.f, 2.5f, 0.4f, 0.1f, EaseType::InQuad, EaseType::InOutQuad);

	switch ((*m_pResultDesc)[m_iIndex].Grade)
	{
	case GachaGrade::S:
		SetLightEffect(_float4(1.0f, 0.9f, 0.2f, 1.0f));
		break;
	case GachaGrade::A:
		SetLightEffect(_float4(1.0f, 0.2f, 0.5f, 1.0f));
		break;
	case GachaGrade::B:
		SetLightEffect(_float4(0.1f, 0.3f, 0.9f, 1.0f));
		break;
	}
}

HRESULT CGachaStage::Initialize_Prototype(vector<GACHA_RESULT_DESC>* Desc)
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    auto pModel = Add_Component<CStaticModel>();
    auto pMaterial = Add_Component<CMaterial>();
   Add_Component<CCollider>();

	pModel->Link_Model("Gacha_Level", "AvatarScreen1out.model");
	pMaterial->Link_Material("Gacha_Level", "AvatarScreen1out.mat");

	m_pResultDesc = Desc;

    return S_OK;
}

HRESULT CGachaStage::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Add_StageScreen();
	
	return S_OK; 
}

void CGachaStage::Awake()
{
	m_MainSpotLightHandle = 
		CDataBase::GetInstance()->Get_CashedData("Gacha")->GetDataByDataIndex(10, MAPOBJ_TYPE::LIGHT)->Handle;
	m_MainPointLightHandle =
		CDataBase::GetInstance()->Get_CashedData("Gacha")->GetDataByDataIndex(11, MAPOBJ_TYPE::LIGHT)->Handle;

	SetLightOff();
}

void CGachaStage::Priority_Update(_float dt)
{
	CObjectContainer* pObjectContainer = Get_Component<CObjectContainer>();
	pObjectContainer->Priority_UpdateChild(dt);
}

void CGachaStage::Update(_float dt)
{
	CObjectContainer* pObjectContainer = Get_Component<CObjectContainer>();
	pObjectContainer->UpdateChild(dt);

	Update_CamTime();

	if (InputDevice()->Key_Tap(VK_SPACE))
	{
		if (m_iIndex == -1) CamDirector()->RequestSequence("Gacha/Spin_Half");
		else CamDirector()->RequestSequence("Gacha/Spin");
		++m_iIndex;
		if (m_iIndex >= m_iMaxIndex) m_iIndex = 0;
	}
}

void CGachaStage::Late_Update(_float dt)
{
	CObjectContainer* pObjectContainer = Get_Component<CObjectContainer>();
	pObjectContainer->Late_UpdateChild(dt);
}

void CGachaStage::Add_StageScreen()
{
	PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_GachaStageScreen", CGachaStageScreen::Create());
	PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_GachaWeapon", CGachaWeapon::Create());
	PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_GachaAvatar", CGachaAvatar::Create());

	auto pObjectContainer = Add_Component<CObjectContainer>();
	COLLIDER_DESC colliderDesc{};
	colliderDesc.eType = COLLIDER_TYPE::BOX;
	colliderDesc.eGroup = COLLISION_GROUP::COMMON;
	colliderDesc.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER);
	colliderDesc.bAutoFit = true;
	colliderDesc.bTrigger = true;

	CGameObject* gachaStageScreen = Builder::Create_Object({ "Gacha_Level", "Proto_GameObject_GachaStageScreen" })
		.Collider(colliderDesc)
		.Build("Screen");

	m_pScreen = dynamic_cast<CGachaStageScreen*>(gachaStageScreen);

	pObjectContainer->Add_Child(gachaStageScreen, true);

	CGameObject* gachaWeapon = Builder::Create_Object({ "Gacha_Level", "Proto_GameObject_GachaWeapon" })
		.Collider(colliderDesc)
		.Position(_float3(0.f, 1.45f, -1.5f))
		.Scale(_float3(2.f, 2.f, 2.f))
		.Build("Weapon");
	gachaWeapon->Get_Component<CTransform>()->Set_Quaternion(_vector4(-0.03, 0.96, -0.11, 0.24));

	m_pWeaponResult = dynamic_cast<CGachaResult*>(gachaWeapon);

	pObjectContainer->Add_Child(gachaWeapon, true);

	RIGIDBODY_DESC rigidDesc{};
	rigidDesc.isKinematic = false;
	rigidDesc.bEnableGravity = true;
	
	colliderDesc = {};
	colliderDesc.eType = COLLIDER_TYPE::BOX;
	colliderDesc.eGroup = COLLISION_GROUP::PLAYER;
	colliderDesc.iCollisionMask = ENUM(COLLISION_GROUP::COMMON);
	colliderDesc.bAutoFit = false;
	colliderDesc.vCenter = { 0.f, 1.f, 0.f };
	colliderDesc.vSize = { 1.f, 2.f, 1.f }; 

	CGameObject* gachaAvatar = Builder::Create_Object({ "Gacha_Level", "Proto_GameObject_GachaAvatar" })
		.Position(_float3(0.f, 1.f, -1.6f))
		.Collider(colliderDesc)
		.RigidBody(rigidDesc)
		.Build("Avatar");

	m_pAvatarResult = dynamic_cast<CGachaResult*>(gachaAvatar);

	pObjectContainer->Add_Child(gachaAvatar, true);
}

void CGachaStage::Set_Stage(GACHA_STAGE eStage)
{
	auto pModel = Get_Component<CStaticModel>();
	auto pMaterial = Get_Component<CMaterial>();

	if (eStage == GACHA_STAGE::AVATAR)
	{
		pModel->Link_Model("Gacha_Level", "AvatarScreen1out.model");
		pMaterial->Link_Material("Gacha_Level", "AvatarScreen1out.mat");
	}
	else
	{
		pModel->Link_Model("Gacha_Level", "BangBooNoScreen1.model");
		pMaterial->Link_Material("Gacha_Level", "BangBooNoScreen1.mat");

		pModel->Hide_MehsByName("0023_GachaStage_Prop_TV_04_mesh0023");
	}
}

void CGachaStage::Update_CamTime()
{
	if (CamDirector()->GetCurSeqName() == "Gacha/Spin_Half")
	{
		if (CamDirector()->GetSeqPlayer()->GetTime() >= 0.7f)
		{
			PlayStageSpin(m_iIndex);
		}
	}
	else if (CamDirector()->GetCurSeqName() == "Gacha/Spin")
	{
		if (CamDirector()->GetSeqPlayer()->GetTime() >= 1.f)
		{
			PlayStageSpin(m_iIndex);
		}
	}
}

//void CGachaStage::SetLightEffect(_float4 color)
//{
//	if (m_MainLightHandle.isValid() == false)
//		return;
//	auto pLight = m_MainLightHandle.Get()->Get_Component<CLight>();
//
//	pLight->Set_CompActive(true);
//	LIGHT_DESC Desc = pLight->SnapShot_Desc();
//	Desc.vLightDiffuse = color;
//	pLight->Set_Desc(Desc);
//}
//
//void CGachaStage::SetLightOff()
//{
//	if (m_MainLightHandle.isValid() == false)
//		return;
//	auto pLight = m_MainLightHandle.Get();
//	pLight->Get_Component<CLight>()->Set_CompActive(false);
//}

void CGachaStage::Update_StageEnviroment(_int index)
{
	GACHA_RESULT_DESC CurrentDesc = (*m_pResultDesc)[index];
	if (CurrentDesc.Grade == GachaGrade::S)
	{
		m_pScreen->SetScreen(GACHA_STAGE::AVATAR, CurrentDesc.Grade);
		Set_Stage(GACHA_STAGE::AVATAR);

		m_pAvatarResult->SetResult(CurrentDesc);

		m_pAvatarResult->SetRenderState(true);
		m_pWeaponResult->SetRenderState(false);
	}
	else
	{
		m_pScreen->SetScreen(GACHA_STAGE::BANGBOO, CurrentDesc.Grade);
		Set_Stage(GACHA_STAGE::BANGBOO);
		m_pWeaponResult->SetResult(CurrentDesc);

		m_pAvatarResult->SetRenderState(false);
		m_pWeaponResult->SetRenderState(true);
	}
}

CGachaStage* CGachaStage::Create(vector<GACHA_RESULT_DESC>* Desc)
{
	CGachaStage* Instance = new CGachaStage();
	if (FAILED(Instance->Initialize_Prototype(Desc)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CGachaStage::Clone(INIT_DESC* pArg)
{
	CGachaStage* Instance = new CGachaStage(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CGachaStage::Free()
{
	__super::Free();
}
