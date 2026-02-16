#include "pch.h"
#include "GachaStage.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "AudioSource.h"
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

#include "UI_GachaTextReveal.h"
#include "EffectContainer.h"

#include "PostRenderer.h"
#include "PostProcessCommand.h"

CGachaStage::CGachaStage()
    :CGameObject()
{
}

CGachaStage::CGachaStage(const CGachaStage& rhs)
    :CGameObject(rhs), m_pResultDesc(rhs.m_pResultDesc)
{
}

void CGachaStage::ShowResults()
{
	m_iIndex = m_iMaxIndex - 1;
	UIDirector()->Show_GachaResult(m_pResultDesc);
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
	
	m_RevealEffectFlow.Start();

	switch ((*m_pResultDesc)[m_iIndex].Grade)
	{
	case GachaGrade::S:
	{
		m_pScreen->SetScreen(GACHA_STAGE::AVATAR, GachaGrade::S);
		SetBottomLightEffect(_float4(0.f, 0.f, 0.f, 0.f), _float4(1.0f, 0.9f, 0.2f, 1.0f), _float4(0.25f, 0.25f, 0.25f, 0.5f), 1.0f);
		SetTopLightEffect(_float4(0.f, 0.f, 0.f, 0.f), _float4(1.0f, 0.9f, 0.2f, 1.0f), _float4(0.25f, 0.25f, 0.25f, 0.5f), 1.2f);
		SetMiddleLightEffect(_float4(0.1f, 0.1f, 0.1f, 1.f), _float4(1.0f, 1.0f, 1.0f, 1.f), 0.6f);

		/* Effect */
		auto pDistortion = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("gacha_distortion.json")
			.Position(_float3(0.f, 0.3f, -3.5f))
			.Build("Gacha_Distortion");

		auto pParticle = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("gacha_s_rank.json")
			.Position(_float3(0.f, 0.5f, -1.f))
			.Build("Gacha_Particle");

		ObjectManager()->Add_Object(pDistortion, { "Gacha_Level","Effect_Layer" });
		ObjectManager()->Add_Object(pParticle, { "Gacha_Level","Effect_Layer" });

	}break;
	case GachaGrade::A:
	{
		m_pScreen->SetScreen(GACHA_STAGE::BANGBOO, GachaGrade::A);
		SetBottomLightEffect(_float4(0.f, 0.f, 0.f, 0.f), _float4(1.0f, 0.2f, 0.5f, 1.0f), _float4(0.25f, 0.25f, 0.25f, 0.5f), 1.0f);
		SetTopLightEffect(_float4(0.f, 0.f, 0.f, 0.f), _float4(1.0f, 0.2f, 0.5f, 1.0f), _float4(0.25f, 0.25f, 0.25f, 0.5f), 1.2f);
		SetMiddleLightEffect(_float4(0.1f, 0.1f, 0.1f, 1.f), _float4(1.0f, 1.0f, 1.0f, 1.f), 0.6f);

		auto pParticle = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("gacha_a_rank.json")
			.Position(_float3(0.f, 0.5f, -1.f))
			.Build("Gacha_Particle");

		ObjectManager()->Add_Object(pParticle, { "Gacha_Level","Effect_Layer" });

		break;
	}
	case GachaGrade::B:
	{
		m_pScreen->SetScreen(GACHA_STAGE::BANGBOO, GachaGrade::B);
		SetBottomLightEffect(_float4(0.f, 0.f, 0.f, 0.f), _float4(0.1f, 0.3f, 0.9f, 1.0f), _float4(0.25f, 0.25f, 0.25f, 0.5f), 1.0f);
		SetTopLightEffect(_float4(0.f, 0.f, 0.f, 0.f), _float4(0.1f, 0.3f, 0.9f, 1.0f), _float4(0.25f, 0.25f, 0.25f, 0.5f), 1.2f);
		SetMiddleLightEffect(_float4(0.1f, 0.1f, 0.1f, 1.f), _float4(1.0f, 1.0f, 1.0f, 1.f), 0.6f);

		auto pParticle = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("gacha_b_rank.json")
			.Position(_float3(0.f, 0.5f, -1.f))
			.Build("Gacha_Particle");

		ObjectManager()->Add_Object(pParticle, {  "Gacha_Level","Effect_Layer" });

		break;
	}
	}
	CUIDirector::GetInstance()->Show_GachaLabel((*m_pResultDesc)[m_iIndex].strLabel);
	if (m_pUITextReveal)
		m_pUITextReveal->Show((*m_pResultDesc)[m_iIndex]);
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

	Add_Component<CAudioSource>();
	Get_Component<CAudioSource>()->SoundFolder(G_GlobalLevelKey, "../Bin/Resources/Gacha/Sound/");

    return S_OK;
}

HRESULT CGachaStage::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Add_StageScreen();
	Add_UIText();
	BaseHalfEffectFlowSetting();
	BaseFullEffectFlowSetting();
	BaseRevealEffectFlowSetting();
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

	m_HalfEffectFlow.Tick(dt);
	m_FullEffectFlow.Tick(dt);
	m_RevealEffectFlow.Tick(dt);
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
		else
		{
			if ((*m_pResultDesc)[m_iIndex].Type == GachaType::Agent)
				Get_Component<CAudioSource>()->Slot("GachaAgent.wav").Play();
			else
				Get_Component<CAudioSource>()->Slot("GachaEngine.wav").Play();
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

void CGachaStage::BaseHalfEffectFlowSetting()
{
 	size_t sequenceId = m_HalfEffectFlow.BeginSequence();
	auto pPost = RenderSystem()->GetPostRenderer();

	m_HalfEffectFlow.AddWait(sequenceId, 0.1);

	m_HalfEffectFlow.AddOnce(sequenceId, [pPost]() {
		pPost->GetCommand<CGuassianBlurCommand>()
			->SetDuration(1.f)
			->SetIntensity(4.f)
			->SetEaseType(EaseType::OutCubic)
			->SetEnable(true);
		});

	m_HalfEffectFlow.AddWait(sequenceId, 0.4);

	auto pTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, "VX_Noise_UU_26.png");
	m_HalfEffectFlow.AddOnce(sequenceId, [pPost, pTexture]() {
		pPost->GetCommand<CGlitchCommand>()
			->SetDuration(0.1f)
			->SetIntensity(0.5f)
			->SetNoiseTexture(pTexture)
			->SetEaseType(EaseType::OutQuart)
			->SetEnable(true);
		});

	m_HalfEffectFlow.AddWait(sequenceId, 0.1);

	m_HalfEffectFlow.AddOnce(sequenceId, [pPost, pTexture]() {
		pPost->GetCommand<CGlitchCommand>()
			->SetDuration(0.1f)
			->SetIntensity(2.f)
			->SetNoiseTexture(pTexture)
			->SetEaseType(EaseType::OutQuart)
			->SetEnable(true);
		});

	m_HalfEffectFlow.AddWait(sequenceId, 0.1);

	m_HalfEffectFlow.AddOnce(sequenceId, [pPost, pTexture]() {
		pPost->GetCommand<CGlitchCommand>()
			->SetDuration(0.1f)
			->SetIntensity(0.5f)
			->SetNoiseTexture(pTexture)
			->SetEaseType(EaseType::OutQuart)
			->SetEnable(true);
		});


	m_HalfEffectFlow.AddOnce(sequenceId, [this, pPost]() {
		pPost->GetCommand<CAddictiveColorCommand>()
			->SetAddictiveColor(&m_vRevealColor)
			->SetSkinned(false)
			->SetEnable(true);
		SetRevealColorEffect(_float4(1.f, 1.f, 1.f, 1.f), _float4(1.f, 1.f, 1.f, 1.f), _float4(0.2f, 0.2f, 0.2f, 1.f), 1.f);
		});

	m_HalfEffectFlow.EndSequence(sequenceId);
}

void CGachaStage::BaseFullEffectFlowSetting()
{
	size_t sequenceId = m_FullEffectFlow.BeginSequence();
	auto pPost = RenderSystem()->GetPostRenderer();

	m_FullEffectFlow.AddWait(sequenceId, 0.1);

	m_FullEffectFlow.AddOnce(sequenceId, [pPost]() {
		pPost->GetCommand<CGuassianBlurCommand>()
			->SetDuration(1.4f)
			->SetIntensity(4.f)
			->SetEaseType(EaseType::InCubic)
			->SetEnable(true);
		});

	m_FullEffectFlow.AddWait(sequenceId, 0.7);

	auto pTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, "VX_Noise_UU_26.png");
	m_FullEffectFlow.AddOnce(sequenceId, [pPost, pTexture]() {
		pPost->GetCommand<CGlitchCommand>()
			->SetDuration(0.3f)
			->SetIntensity(0.5f)
			->SetNoiseTexture(pTexture)
			->SetEaseType(EaseType::OutQuart)
			->SetEnable(true);
		});

	m_FullEffectFlow.AddWait(sequenceId, 0.3);

	m_FullEffectFlow.AddOnce(sequenceId, [pPost, pTexture]() {
		pPost->GetCommand<CGlitchCommand>()
			->SetDuration(0.1f)
			->SetIntensity(2.f)
			->SetNoiseTexture(pTexture)
			->SetEaseType(EaseType::OutQuart)
			->SetEnable(true);
		});

	m_FullEffectFlow.AddWait(sequenceId, 0.1);

	m_FullEffectFlow.AddOnce(sequenceId, [pPost, pTexture]() {
		pPost->GetCommand<CGlitchCommand>()
			->SetDuration(0.3f)
			->SetIntensity(0.5f)
			->SetNoiseTexture(pTexture)
			->SetEaseType(EaseType::OutQuart)
			->SetEnable(true);
		});

	m_FullEffectFlow.AddOnce(sequenceId, [this, pPost]() {
		pPost->GetCommand<CAddictiveColorCommand>()
			->SetAddictiveColor(&m_vRevealColor)
			->SetSkinned(false)
			->SetEnable(true);
		SetRevealColorEffect(_float4(1.f, 1.f, 1.f, 1.f), _float4(1.f, 1.f, 1.f, 1.f), _float4(0.2f, 0.2f, 0.2f, 1.f), 1.f);
		});

	m_FullEffectFlow.EndSequence(sequenceId);
}

void CGachaStage::BaseRevealEffectFlowSetting()
{
	size_t sequenceId = m_RevealEffectFlow.BeginSequence();
	auto pPost = RenderSystem()->GetPostRenderer();

	m_RevealEffectFlow.AddOnce(sequenceId, [this, pPost]() {
		SetRevealColorEffect(_float4(0.2f, 0.2f, 0.2f, 1.f), _float4(1.f, 1.f, 1.f, 1.f), _float4(1.f, 1.f, 1.f, 1.f), 0.2f);
		});

	m_RevealEffectFlow.AddWait(sequenceId, 0.4);

	m_RevealEffectFlow.AddOnce(sequenceId, [pPost]() {
		pPost->GetCommand<CGuassianBlurCommand>()
			->SetDuration(0.4f)
			->SetIntensity(5.f)
			->SetEaseType(EaseType::InCubic)
			->SetEnable(true);
		});


	m_RevealEffectFlow.AddOnce(sequenceId, [this, pPost]() {
		pPost->GetCommand<CAddictiveColorCommand>()
			->SetEnable(false);	
		});

	m_RevealEffectFlow.EndSequence(sequenceId);
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
	PrototypeManager()->Add_ProtoType("Gacha_Level", "Proto_GameObject_UIGachaTextReveal", CUI_GachaTextReveal::Create());
	
	CGameObject* uiGachaText = Builder::Create_Object({ "Gacha_Level", "Proto_GameObject_UIGachaTextReveal" })
		.Rotate(_float3(0.f, XMConvertToRadians(180.f), 0.f))
		.Position(_float3(0.f, 0.f, -1.f))
		.Build("uiGachaText");
	
	Get_Component<CObjectContainer>()->Add_Child(uiGachaText);
	m_pUITextReveal = dynamic_cast<CUI_GachaTextReveal*>(uiGachaText);
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
	if(auto pLight = CamDirector()->GetSeqCam()->Get_Component<CLight>())
		pLight->Set_CompActive(false);
}

void CGachaStage::Play_CameraSequence()
{
	Reset_Target();
	if (m_iIndex == -1)
	{
		CamDirector()->RequestSequence("Gacha/Spin_Half");
		m_HalfEffectFlow.Start();
	}
	else if (m_iIndex < m_iMaxIndex - 1) 
	{
		CamDirector()->RequestSequence("Gacha/Spin");
		m_FullEffectFlow.Start();
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

	if (AddictiveColor.Duration > 0.f)
	{
		AddictiveColor.CurElpasedTime += dt;

		if (AddictiveColor.CurElpasedTime <= AddictiveColor.Duration)
		{
			_float fProgress = AddictiveColor.CurElpasedTime / AddictiveColor.Duration;
			_float fEasedRatio = Math::ApplyEase(EaseType::InOutSine, fProgress);

			_vector4 vCurrentColor = XMVectorLerp(AddictiveColor.StartColor, AddictiveColor.EndColor, fEasedRatio);
			m_vRevealColor = _vector3(vCurrentColor);
		}
		else
		{
			m_vRevealColor = _vector3(AddictiveColor.EndColor);
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

void CGachaStage::SetRevealColorEffect(_float4 Start, _float4 Middle, _float4 End, _float Duration)
{
	AddictiveColor = { Start, Middle, End, 0.f,  Duration };
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
