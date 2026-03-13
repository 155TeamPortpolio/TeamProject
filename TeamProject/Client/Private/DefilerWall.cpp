#include "pch.h"
#include "DefilerWall.h"

#include "BattleSystem.h"
#include "GameInstance.h"

#include "StaticModel.h"
#include "Material.h"
#include "Collider.h"
#include "RigidBody.h"
#include "ObjectContainer.h"

#include "Helper_Func.h"
#include "Character.h"
#include "Defiler.h"
#include "Texture.h"
#include "AudioSource.h"
#include "EventListener.h"

#include "EffectContainer.h"

CDefilerWall::CDefilerWall()
	: CEnemy()
{
}

CDefilerWall::CDefilerWall(const CDefilerWall& rhs)
	:CEnemy(rhs)
{
}

HRESULT CDefilerWall::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CStaticModel>()->Link_Model("Zero_Level", "Defiler_Wall.model");
	Add_Component<CMaterial>()->Link_Material("Zero_Level", "Defiler_Wall.mat");
	Add_Component<CCollider>();
	Add_Component<CEventListener>();
	//Add_Component<CRigidBody>();
	Add_Component<CAudioSource>();
	return S_OK;
}

HRESULT CDefilerWall::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto desc = static_cast<DefilerWallDesc*>(pArg);
	m_pTransform->Set_Look(desc->vLook);
	auto pMaterial = Get_Component<CMaterial>();
	auto& materialInstances = pMaterial->Get_MaterialInstances();
	auto Texture =ResourceManager()->Load_Texture(G_GlobalLevelKey, "Eff_AZX_010.png");
	for (const auto& instance : materialInstances)
	{
		instance->Override_Pass("Opaque");
		instance->Set_Param("fTime",{ &m_ElapsedTime,"float",  sizeof(_float) });
		instance->Set_Param("DissolveTexture",{ Texture->Get_SRV(),"Texture2D",  0});
	}

	m_pTransform->Scale({ 0,0,0 });
	m_EndY = -2.3f;
	m_pTransform->Set_Y(-5.f);
	m_bAwake = true;
	Get_Component<CEventListener>()->Add_Listener<TsunamiDesc>([&](TsunamiDesc desc) {DisAppear(desc.isEndTsunami);});
	Get_Component<CEventListener>()->Add_Listener<TsunamiDesc>([&](TsunamiDesc desc) {Play_Effect(desc.isHitGround); });
	Get_Component<CAudioSource>()->SoundFolder("Zero_Level", "../Bin/Resources/Zero/Enemy/Defiler_Isolde/Sound/");

	Initialize_Effects();

	return S_OK;
}

void CDefilerWall::Awake()
{

}

void CDefilerWall::Priority_Update(_float dt)
{
}

void CDefilerWall::Update(_float dt)
{
	m_ElapsedTime += dt;
	if (m_bAwake) {
		const _float duration = .45f;
		const _float t01 = clamp(m_ElapsedTime / duration, 0.f, 1.f);
		const _float eased = Math::ApplyEase(EaseType::OutBack, t01);
		const _vector3 startScale = { 0.2f, 0.05f, 0.2f };
		const _vector3 endScale = { 1.f, 1.f, 1.f };
		const _float startY = -5.f;
		const _float endY = m_EndY;
		const _vector3 scale = startScale + (endScale - startScale) * eased;
		const _float y = Math::Lerp(startY, endY, eased);
		m_pTransform->Scale(scale);
		m_pTransform->Set_Y(y);
		if (t01 >= 1.f)
		{
			m_pTransform->Scale(endScale);
			m_pTransform->Set_Y(endY);
			m_bAwake = false;
		}
	}
	else if (m_bDisApper) {
		if (m_ElapsedTime > 4.f)
			ObjectManager()->Remove_Object(this);
	}

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CDefilerWall::Late_Update(_float dt)
{
}

void CDefilerWall::Render_GUI()
{
	__super::Render_GUI();
}

void CDefilerWall::OnPooledAcquire(INIT_DESC* pArg)
{
}

void CDefilerWall::OnPooledRelease()
{
	
}

void CDefilerWall::DisAppear(_bool isDisappear)
{
	if (!isDisappear)
		return;
	auto pMaterial = Get_Component<CMaterial>();
	auto& materialInstances = pMaterial->Get_MaterialInstances();
	m_ElapsedTime = 0.f;
	for (const auto& instance : materialInstances)
	{
		instance->Override_Pass("Wall");
	}
	m_bDisApper = true;

}

void CDefilerWall::Play_Effect(_bool isPlayEffect)
{
	if (!isPlayEffect)
		return;

	if (m_bPlayEffect)
		return;

	auto pEffect = Get_Component<CObjectContainer>()->Find_ObjectByName("Defiler_Wave_HitGround");
	if (pEffect)
		static_cast<CEffectContainer*>(pEffect)->Play();

	Get_Component<CAudioSource>()->Slot("DefilerExplode01.wav").Volume(0.3f).Play();
	Get_Component<CAudioSource>()->Slot("DefilerExplode02.wav").Volume(0.5f).Play();
	m_bPlayEffect = true;
}

void CDefilerWall::Initialize_Effects()
{
	auto pObjectContainer = Get_Component<CObjectContainer>();

	auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
		.Asset("defiler_wave_hit_ground.json")
		.Position(_float3(0.f, 0.f, -4.f))
		.Build("Defiler_Wave_HitGround");
	pEffect->Get_Component<CTransform>()->Rotate(_float3(0.f, XMConvertToRadians(90.f), 0.f));
		
	pEffect->Stop();
	pObjectContainer->Add_Child(pEffect);
}

CDefilerWall* CDefilerWall::Create()
{
	CDefilerWall* instance = new CDefilerWall();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CDefilerWall");
	}

	return instance;
}

CGameObject* CDefilerWall::Clone(INIT_DESC* pArg)
{
	CDefilerWall* instance = new CDefilerWall(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CDefilerWall");
	}

	return instance;
}

void CDefilerWall::Free()
{
	__super::Free();
}

void CDefilerWall::OnTriggerEnter(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (!pCollidable)
		return;

	else {
		auto pEnemy = dynamic_cast<CCharacter*>(pOther);
		if (nullptr != pEnemy)
		{
			EventSystem()->Broadcast<TsunamiWallDesc>({true});
		}
	}
}

void CDefilerWall::OnTriggerExit(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (!pCollidable)
		return;

	else {
		auto pEnemy = dynamic_cast<CCharacter*>(pOther);
		if (nullptr != pEnemy)
		{
			EventSystem()->Broadcast<TsunamiWallDesc>({ false });
		}
	}
}
