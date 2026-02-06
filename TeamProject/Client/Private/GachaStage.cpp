#include "pch.h"
#include "GachaStage.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Child.h"
#include "StaticModel.h"
#include "Material.h"
#include "Light.h"

#include "GachaProps.h"
#include "GachaStageScreen.h"
#include "GachaWeapon.h"
#include "GachaAvatar.h"
#include "GachaFootStage.h"

#include "CamDirector.h"
#include "DataBase.h"
#include "UIDirector.h"

#include "Helper_Func.h"

#include "UI_GachaText.h"

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
	SetInitLight();
	Update_StageEnviroment(m_iSpinIndex);
	m_pScreen->ScreenOff(m_eStage);
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
		m_pScreen->SetScreen(GACHA_STAGE::AVATAR, GachaGrade::S);
		SetBottomLightEffect(_float4(0.f,0.f,0.f,0.f), _float4(1.0f, 0.9f, 0.2f, 1.0f), _float4(0.25f, 0.25f, 0.25f, 0.5f), 1.0f);
		SetTopLightEffect(_float4(0.f, 0.f, 0.f, 0.f), _float4(1.0f, 0.9f, 0.2f, 1.0f), _float4(0.25f, 0.25f, 0.25f, 0.5f), 1.2f);
		SetMiddleLightEffect(_float4(0.1f, 0.1f, 0.1f, 1.f), _float4(1.0f, 1.0f, 1.0f, 1.f), 0.6f);
		break;
	case GachaGrade::A:
		m_pScreen->SetScreen(GACHA_STAGE::BANGBOO, GachaGrade::A);
		SetBottomLightEffect(_float4(0.f, 0.f, 0.f, 0.f), _float4(1.0f, 0.2f, 0.5f, 1.0f), _float4(0.25f, 0.25f, 0.25f, 0.5f), 1.0f);
		SetTopLightEffect(_float4(0.f, 0.f, 0.f, 0.f), _float4(1.0f, 0.2f, 0.5f, 1.0f), _float4(0.25f, 0.25f, 0.25f, 0.5f), 1.2f);
		SetMiddleLightEffect(_float4(0.1f, 0.1f, 0.1f, 1.f), _float4(1.0f, 1.0f, 1.0f, 1.f), 0.6f);
		break;
	case GachaGrade::B:
		m_pScreen->SetScreen(GACHA_STAGE::BANGBOO, GachaGrade::B);
		SetBottomLightEffect(_float4(0.f, 0.f, 0.f, 0.f), _float4(0.1f, 0.3f, 0.9f, 1.0f), _float4(0.25f, 0.25f, 0.25f, 0.5f), 1.0f);
		SetTopLightEffect(_float4(0.f, 0.f, 0.f, 0.f), _float4(0.1f, 0.3f, 0.9f, 1.0f), _float4(0.25f, 0.25f, 0.25f, 0.5f), 1.2f);
		SetMiddleLightEffect(_float4(0.1f, 0.1f, 0.1f, 1.f), _float4(1.0f, 1.0f, 1.0f, 1.f), 0.6f);
		break;
	}
	CUIDirector::GetInstance()->Show_GachaLabel((*m_pResultDesc)[m_iIndex].strLabel);
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
	Add_UIText();

	return S_OK; 
}

void CGachaStage::Awake()
{
	m_MainBottomLightHandle = 
		CDataBase::GetInstance()->Get_CashedData("Gacha")->GetDataByDataIndex(10, MAPOBJ_TYPE::LIGHT)->Handle;
	m_MainTopLightHandle =
		CDataBase::GetInstance()->Get_CashedData("Gacha")->GetDataByDataIndex(11, MAPOBJ_TYPE::LIGHT)->Handle;
	m_MainMiddleLightHandle =
		CDataBase::GetInstance()->Get_CashedData("Gacha")->GetDataByDataIndex(18, MAPOBJ_TYPE::LIGHT)->Handle;

	SetInitLight();
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
	Update_Lights(dt);

	if (InputDevice()->Key_Tap(VK_SPACE))
	{
		Play_CameraSequence();

		++m_iIndex;
		if (m_iIndex >= m_iMaxIndex)
		{
			m_iIndex = m_iMaxIndex - 1;
			UIDirector()->Show_GachaResult(m_pResultDesc);
		}

		CUIDirector::GetInstance()->Hide_GachaLabel();
		SetMiddleLightEffect(_float4(1.f, 1.f, 1.f, 1.f), _float4(0.1f, 0.1f, 0.1f, 1.f), 1.f);
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
	PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_GachaFootStage", CGachaFootStage::Create());

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

	colliderDesc = {};
	colliderDesc.eType = COLLIDER_TYPE::BOX;
	colliderDesc.eGroup = COLLISION_GROUP::COMMON;
	colliderDesc.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER);
	colliderDesc.bAutoFit = true;

	CGameObject* gachaFootStage = Builder::Create_Object({ "Gacha_Level", "Proto_GameObject_GachaFootStage" })
		.Position(_float3(0.f, 0.3f, -1.4f))
		.Collider(colliderDesc)
		.Build("FootStage");

	m_pFootStage = dynamic_cast<CGachaFootStage*>(gachaFootStage);

	pObjectContainer->Add_Child(gachaFootStage, true);

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
		.Position(_float3(0.f, 1.5f, -1.6f))
		.Collider(colliderDesc)
		.RigidBody(rigidDesc)
		.Build("Avatar");

	m_pAvatarResult = dynamic_cast<CGachaResult*>(gachaAvatar);

	pObjectContainer->Add_Child(gachaAvatar, true);
}

void CGachaStage::Add_UIText()
{
	PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_UIGachaText", CUI_GachaText::Create());

	CGameObject* uiGachaText = Builder::Create_Object({ "Gacha_Level", "Proto_GameObject_UIGachaText" })
		.Rotate(_float3(XMConvertToRadians(-18.f), XMConvertToRadians(180.f), 0.f))
		.Scale(_float3(1.28f, 1.28f, 1.28f))
		.Position(_float3(0.f, 0.152f, -2.5f))
		.Build("uiGachaText");

	Add_Component<CObjectContainer>()->Add_Child(uiGachaText, true);
}

void CGachaStage::Set_Stage(GACHA_STAGE eStage, _int ResultID)
{
	auto pModel = Get_Component<CStaticModel>();
	auto pMaterial = Get_Component<CMaterial>();

	m_eStage = eStage;
	if (eStage == GACHA_STAGE::AVATAR)
	{
		pModel->Link_Model("Gacha_Level", "AvatarScreen1out.model");
		pMaterial->Link_Material("Gacha_Level", "AvatarScreen1out.mat");
		if (ResultID != 14)
		{
			m_pFootStage->SetRenderLayer(RENDER_LAYER::Default);
			m_pFootStage->Get_Component<CCollider>()->Set_CollisionMask(ENUM(COLLISION_GROUP::PLAYER));
		}
	}
	else
	{
		pModel->Link_Model("Gacha_Level", "BangBooNoScreen1.model");
		pMaterial->Link_Material("Gacha_Level", "BangBooNoScreen1.mat");
		pModel->Hide_MehsByName("0023_GachaStage_Prop_TV_04_mesh0023");
		m_pFootStage->SetRenderLayer(RENDER_LAYER::None);
		m_pFootStage->Get_Component<CCollider>()->Set_CollisionMask(ENUM(COLLISION_GROUP::COMMON));
	}
}

void CGachaStage::Reset_Target()
{
	dynamic_cast<CGachaProps*>(Get_Component<CChild>()->Get_Parent())->ResetTarget();
}

void CGachaStage::Play_CameraSequence()
{
	Reset_Target();

	if (m_iIndex == -1) CamDirector()->RequestSequence("Gacha/Spin_Half");
	else if (m_iIndex < m_iMaxIndex - 1) CamDirector()->RequestSequence("Gacha/Spin");
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

void CGachaStage::Update_Lights(_float dt)
{
	if (m_MainBottomLightHandle.isValid() && BottomLight.Duration > 0.f)
	{
		BottomLight.CurElpasedTime += dt;

		if (BottomLight.CurElpasedTime <= BottomLight.Duration)
		{
			auto pLight = m_MainBottomLightHandle.Get()->Get_Component<CLight>();
			LIGHT_DESC Desc = pLight->Get_Desc();

			_float fProgress = BottomLight.CurElpasedTime / BottomLight.Duration;
			_vector4 vCurrentColor;

			if (fProgress <= 0.5f)
			{
				_float t = fProgress * 2.f; 
				_float fEasedRatio = Math::ApplyEase(EaseType::OutExpo, t);
				vCurrentColor = XMVectorLerp(BottomLight.StartColor, BottomLight.MiddleColor, fEasedRatio);
			}
			else
			{
				_float t = (fProgress - 0.5f) * 2.f; 
				_float fEasedRatio = Math::ApplyEase(EaseType::InExpo, t);
				vCurrentColor = XMVectorLerp(BottomLight.MiddleColor, BottomLight.EndColor, fEasedRatio);
			}

			Desc.vLightDiffuse = vCurrentColor;
			pLight->Set_Desc(Desc);
		}
		else
		{
			auto pLight = m_MainBottomLightHandle.Get()->Get_Component<CLight>();
			LIGHT_DESC Desc = pLight->Get_Desc();
			Desc.vLightDiffuse = BottomLight.EndColor;
			pLight->Set_Desc(Desc);
		}
	}

	if (m_MainTopLightHandle.isValid() && TopLight.Duration > 0.f)
	{
		TopLight.CurElpasedTime += dt;

		if (TopLight.CurElpasedTime <= TopLight.Duration)
		{
			auto pLight = m_MainTopLightHandle.Get()->Get_Component<CLight>();
			LIGHT_DESC Desc = pLight->Get_Desc();

			_float fProgress = TopLight.CurElpasedTime / TopLight.Duration;
			_vector4 vCurrentColor;

			if (fProgress <= 0.5f)
			{
				_float t = fProgress * 2.f;
				_float fEasedRatio = Math::ApplyEase(EaseType::OutExpo, t);
				vCurrentColor = XMVectorLerp(TopLight.StartColor, TopLight.MiddleColor, fEasedRatio);
			}
			else
			{
				_float t = (fProgress - 0.5f) * 2.f;
				_float fEasedRatio = Math::ApplyEase(EaseType::InExpo, t);
				vCurrentColor = XMVectorLerp(TopLight.MiddleColor, TopLight.EndColor, fEasedRatio);
			}

			Desc.vLightDiffuse = vCurrentColor;
			pLight->Set_Desc(Desc);
		}
		else
		{
			auto pLight = m_MainTopLightHandle.Get()->Get_Component<CLight>();
			LIGHT_DESC Desc = pLight->Get_Desc();
			Desc.vLightDiffuse = TopLight.EndColor;
			pLight->Set_Desc(Desc);
		}
	}

	if (m_MainMiddleLightHandle.isValid() && MiddleLight.Duration > 0.f)
	{
		MiddleLight.CurElpasedTime += dt;

		if (MiddleLight.CurElpasedTime <= MiddleLight.Duration)
		{
			auto pLight = m_MainMiddleLightHandle.Get()->Get_Component<CLight>();
			LIGHT_DESC Desc = pLight->Get_Desc();

			_float fProgress = MiddleLight.CurElpasedTime / MiddleLight.Duration;
			_float fEasedRatio = Math::ApplyEase(EaseType::InOutCubic, fProgress);

			_vector4 vCurrentColor = XMVectorLerp(MiddleLight.StartColor, MiddleLight.EndColor, fEasedRatio);
			Desc.vLightDiffuse = vCurrentColor;
			pLight->Set_Desc(Desc);
		}
		else
		{
			auto pLight = m_MainMiddleLightHandle.Get()->Get_Component<CLight>();
			LIGHT_DESC Desc = pLight->Get_Desc();
			Desc.vLightDiffuse = MiddleLight.EndColor;
			pLight->Set_Desc(Desc);
		}
	}

}

void CGachaStage::SetBottomLightEffect(_float4 BottomStartColor, _float4 BottomMiddleColor, _float4 BottomEndColor, _float Duration)
{
	if (m_MainBottomLightHandle.isValid() == false)
		return;
	auto pLight = m_MainBottomLightHandle.Get()->Get_Component<CLight>();

	pLight->Set_CompActive(true);
	LIGHT_DESC Desc = pLight->Get_Desc();
	Desc.vLightDiffuse = BottomStartColor;
	BottomLight = { BottomStartColor, BottomMiddleColor, BottomEndColor, 0.f,  Duration };
	pLight->Set_Desc(Desc);
}

void CGachaStage::SetTopLightEffect(_float4 TopStartColor, _float4 TopMiddleColor, _float4 TopEndColor, _float Duration)
{
	if (m_MainTopLightHandle.isValid() == false)
		return;
	auto pLight = m_MainTopLightHandle.Get()->Get_Component<CLight>();

	pLight->Set_CompActive(true);
	LIGHT_DESC Desc = pLight->Get_Desc();
	Desc.vLightDiffuse = TopStartColor;
	TopLight = { TopStartColor, TopMiddleColor, TopEndColor, 0.f,  Duration };
	pLight->Set_Desc(Desc);
}

void CGachaStage::SetMiddleLightEffect(_float4 MiddleStartColor, _float4 MiddleEndColor, _float Duration)
{
	if (m_MainMiddleLightHandle.isValid() == false)
		return;
	auto pLight = m_MainMiddleLightHandle.Get()->Get_Component<CLight>();

	pLight->Set_CompActive(true);
	LIGHT_DESC Desc = pLight->Get_Desc();
	Desc.vLightAmbient = MiddleStartColor;
	MiddleLight = { MiddleStartColor, _float4(0.f,0.f,0.f,0.f), MiddleEndColor, 0.f,  Duration};
	pLight->Set_Desc(Desc);
}

void CGachaStage::SetInitLight()
{
	if (m_MainTopLightHandle.isValid() == false || m_MainBottomLightHandle.isValid() == false)
		return;

	auto pTopLight = m_MainTopLightHandle.Get();
	pTopLight->Get_Component<CLight>()->Set_CompActive(false);

	auto pBottomLight = m_MainBottomLightHandle.Get();
	pBottomLight->Get_Component<CLight>()->Set_CompActive(false);

	auto pDirectionLight = m_MainMiddleLightHandle.Get()->Get_Component<CLight>();
	pBottomLight->Get_Component<CLight>()->Set_CompActive(true);
}

void CGachaStage::Update_StageEnviroment(_int index)
{
	GACHA_RESULT_DESC CurrentDesc = (*m_pResultDesc)[index];
	if (CurrentDesc.Grade == GachaGrade::S)
	{
		Set_Stage(GACHA_STAGE::AVATAR, CurrentDesc.ID);
		m_pAvatarResult->SetResult(CurrentDesc);

		m_pAvatarResult->SetRenderState(true);
		m_pWeaponResult->SetRenderState(false);
	}
	else
	{
		Set_Stage(GACHA_STAGE::BANGBOO, CurrentDesc.ID);
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
