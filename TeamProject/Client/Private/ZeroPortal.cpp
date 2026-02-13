#include "pch.h"
#include "ZeroPortal.h"
#include "LevelMgr.h"
#include "GameInstance.h"

//Components
#include "ObjectContainer.h"
#include "EventListener.h"
#include "Stage.h"

//Object
#include "EffectContainer.h"
#include "AudioSource.h"

CZeroPortal::CZeroPortal()
	: CInteractable()
{
}

CZeroPortal::CZeroPortal(const CZeroPortal& rhs)
	: CInteractable(rhs)
{
}

HRESULT CZeroPortal::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CObjectContainer>();
	Add_Component<CEventListener>();
	Add_Component<CAudioSource>();

	Get_Component<CAudioSource>()->SoundFolder("Zero_Level", "../Bin/Resources/Zero/Map/Sound/");

	return S_OK;
}

HRESULT CZeroPortal::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	auto pObjectContainer = Get_Component<CObjectContainer>();

	auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
		.Asset("zero_portal.json")
		.Build("ZeroPortal");

	pObjectContainer->Add_Child(pEffect);

	return S_OK;
}

void CZeroPortal::Awake()
{
	m_pTransform->Scale({ 0.15f, 0.15f, 0.15f });
	Get_Component<CCollider>()->Set_Size({ 2.5f, 2.5f, 2.5f });
	Get_Component<CCollider>()->Set_Trigger(true);

	_vector3 vPos = m_pTransform->Get_WorldPos();
	vPos.y += 1.5f;
	m_pTransform->Set_Pos(vPos);

	m_vBaseScale	= m_pTransform->Get_Scale();
	m_vExtendScale	= m_vBaseScale * 3.f;
	m_fDuration		= 0.7f;
}

void CZeroPortal::Priority_Update(_float dt)
{
}

void CZeroPortal::Update(_float dt)
{
	m_Time += dt;
	Get_Component<CCollider>()->Update(dt);
	Get_Component<CObjectContainer>()->UpdateChild(dt);
	Get_Component<CAudioSource>()->Set_AudioPos(Get_WorldPos());

	Focus(dt);
}

void CZeroPortal::Late_Update(_float dt)
{
}

void CZeroPortal::Render_GUI()
{
	__super::Render_GUI();
}

void CZeroPortal::OnTriggerEnter(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;

	m_bIsInteractable = true;
	m_bInPlayer = true;
	Get_Component<CAudioSource>()->Slot("ZeroPortal_Touch.wav")
		.Attribute3D(true)
		.Loop(false)
		.Play();
}

void CZeroPortal::OnTriggerStay(CGameObject* pOher)
{

}

void CZeroPortal::OnTriggerExit(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;

	m_bIsInteractable = false;
	m_bInPlayer = false;
}

void CZeroPortal::Interact(CGameObject* pObject)
{
	if (!m_bIsInteractable)
		return;

	m_pOwnerStage->StageChangeOn(m_choiceIndex);
	m_bIsInteractable = false;
	Get_Component<CAudioSource>()->Slot("ZeroPortal_Enter.wav").Play();
}

OBJECT_HANDLE CZeroPortal::Get_InteractHandle()
{
	return Get_Handle();
}

void CZeroPortal::SetChoiceIndex(CStage* pOwener, int idx)
{
	m_pOwnerStage = pOwener;
	m_choiceIndex = idx;
}

void CZeroPortal::Focus(_float dt)
{
	if (m_bInPlayer)
		m_fElapsedTime += dt;
	else
		m_fElapsedTime -= dt;

	m_fElapsedTime = clamp(m_fElapsedTime, 0.f, m_fDuration);
	_float t = m_fElapsedTime / m_fDuration;

	_vector3 vCurrScale = _vector3::Lerp(_vector3(m_vBaseScale), _vector3(m_vExtendScale), Math::ApplyEase(EaseType::OutExpo, t));

	m_pTransform->Scale(vCurrScale);
}

void CZeroPortal::Extend(_float dt)
{
	m_fElapsedTime += dt;
	if (m_fElapsedTime >= m_fDuration)
	{
		m_OnExtend = false;
	}
	else
	{
		_float t = m_fElapsedTime / m_fDuration;

		_vector3 vCurrScale = _vector3::Lerp(_vector3(m_vBaseScale), _vector3(m_vExtendScale), Math::ApplyEase(EaseType::OutExpo, t));
		m_pTransform->Scale(vCurrScale);
	}
}

void CZeroPortal::Contract(_float dt)
{
	m_fElapsedTime += dt;
	if (m_fElapsedTime >= m_fDuration)
	{
		m_OnContract = false;
	}
	else
	{
		_float t = m_fElapsedTime / m_fDuration;

		_vector3 vCurrScale = _vector3::Lerp(_vector3(m_vExtendScale), _vector3(m_vBaseScale), Math::ApplyEase(EaseType::OutExpo, t));
		m_pTransform->Scale(vCurrScale);
	}
}

CZeroPortal* CZeroPortal::Create()
{
	CZeroPortal* Instance = new CZeroPortal();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CZeroPortal::Clone(INIT_DESC* pArg)
{
	CZeroPortal* Instance = new CZeroPortal(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CZeroPortal::Free()
{
	__super::Free();
}


