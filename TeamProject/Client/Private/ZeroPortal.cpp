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

	Get_Component<CCollider>()->Set_Trigger(true);
}

void CZeroPortal::Priority_Update(_float dt)
{
}

void CZeroPortal::Update(_float dt)
{
	m_Time += dt;
	Get_Component<CCollider>()->Update(dt);
	Get_Component<CObjectContainer>()->UpdateChild(dt);

	if (m_OnExtend)
		Extend(dt);

	if (m_OnContract)
		Contract(dt);

	if (m_bIsInteractable) {
		//Interact();
		//m_vTargetSize = { 3.f,4.f,3.f };
	}
	else {
		//m_vTargetSize = { 1.f,1.f,1.f };
	}
}

void CZeroPortal::Late_Update(_float dt)
{
}

void CZeroPortal::Render_GUI()
{
	__super::Render_GUI();

	if (ImGui::Button("Extend"))
	{
		m_OnExtend = true;
		m_fDuration = 0.7f;
		m_fElapsedTime = 0.f;
	}

	if (ImGui::Button("Contract"))
	{
		m_OnContract = true;
		m_fDuration = 0.7f;
		m_fElapsedTime = 0.f;
	}

}

void CZeroPortal::OnTriggerEnter(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;

	m_bIsInteractable = true;
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
}

void CZeroPortal::Interact(CGameObject* pObject)
{
	if (!m_bIsInteractable)
		return;

	m_pOwnerStage->StageChangeOn(m_choiceIndex);
	m_bIsInteractable = false;
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

void CZeroPortal::Extend(_float dt)
{
	m_fElapsedTime += dt;
	if (m_fElapsedTime >= m_fDuration)
	{
		m_OnExtend = false;
		m_pTransform->Scale(m_vExtendScale);
	}
	else
	{
		_float t = m_fElapsedTime / m_fDuration;

		_vector3 vCurrScale = _vector3::Lerp(_vector3(m_vContractScale), _vector3(m_vExtendScale), Math::ApplyEase(EaseType::OutExpo, t));
		m_pTransform->Scale(vCurrScale);
	}
}

void CZeroPortal::Contract(_float dt)
{
	m_fElapsedTime += dt;
	if (m_fElapsedTime >= m_fDuration)
	{
		m_OnContract = false;
		m_pTransform->Scale(m_vContractScale);
	}
	else
	{
		_float t = m_fElapsedTime / m_fDuration;

		_vector3 vCurrScale = _vector3::Lerp(_vector3(m_vExtendScale), _vector3(m_vContractScale), Math::ApplyEase(EaseType::OutExpo, t));
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


