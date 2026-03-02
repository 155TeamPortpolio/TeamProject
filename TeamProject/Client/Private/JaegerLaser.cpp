#include "pch.h"
#include "JaegerLaser.h"
#include "EffectContainer.h"

// Component
#include "ObjectContainer.h"
#include "BoneFollower.h"
#include "Child.h"

CJaegerLaser::CJaegerLaser()
	:CGameObject()
{
}

CJaegerLaser::CJaegerLaser(const CJaegerLaser& rhg)
	:CGameObject(rhg)
{
}

HRESULT CJaegerLaser::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CObjectContainer>();
	Add_Component<CBoneFollower>();
	return S_OK;
}

HRESULT CJaegerLaser::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	Initialize_Effects();

	auto pBoneFollower = Get_Component<CBoneFollower>();
	pBoneFollower->Initialize(nullptr);

	m_isAlive = false;
	m_fDuration = 0.06f;

	return S_OK;
}

void CJaegerLaser::Awake()
{
}

void CJaegerLaser::Priority_Update(_float dt)
{
}

void CJaegerLaser::Update(_float dt)
{
	Update_Laser(dt);
	Get_Component<CBoneFollower>()->Sync_Transform(dt, m_pTransform);
	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CJaegerLaser::Late_Update(_float dt)
{
}

void CJaegerLaser::Render_GUI()
{
	__super::Render_GUI();
}

void CJaegerLaser::Active_Laser()
{
	m_isAlive = true;
	m_IsDeactive = false;

	auto pObjectContainer = Get_Component<CObjectContainer>();

	auto pLaser0 = pObjectContainer->Find_ObjectByName("Laser0");
	if (pLaser0)
		static_cast<CEffectContainer*>(pLaser0)->Play();

	auto pLaser1 = pObjectContainer->Find_ObjectByName("Laser1");
	if (pLaser1)
		static_cast<CEffectContainer*>(pLaser1)->Play();

	m_fElaspedTime = 0.f;
}

void CJaegerLaser::Deactive_Laser()
{
	m_IsDeactive = true;
	auto pObjectContainer = Get_Component<CObjectContainer>();

	auto pLaser0 = pObjectContainer->Find_ObjectByName("Laser0");
	if (pLaser0)
	{
		static_cast<CEffectContainer*>(pLaser0)->Stop();
	}

	auto pLaser1 = pObjectContainer->Find_ObjectByName("Laser1");
	if (pLaser1)
	{
		static_cast<CEffectContainer*>(pLaser1)->Stop();
	}
}

void CJaegerLaser::Initialize_Effects()
{
	auto pObjectContainer = Get_Component<CObjectContainer>();

	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_jaeger_laser0.json")
			.Build("Laser0");

		if (pEffect)
		{
			pEffect->Stop();
			pObjectContainer->Add_Child(pEffect);
		}
	}

	{
		auto pEffect = Builder::Create_EffectContainer({ G_GlobalLevelKey,"Proto_GameObject_EffectContainer" })
			.Asset("defiler_jaeger_laser1.json")
			.Build("Laser1");

		if (pEffect)
		{
			pEffect->Stop();
			pObjectContainer->Add_Child(pEffect);
		}
	}
}

void CJaegerLaser::Update_Laser(_float dt)
{
	if (m_IsDeactive)
		return;

	_vector3 vStartPosition{}, vTargetPosition{};
	_vector3 vDir = _vector3(m_vTargetPos) - _vector3(m_pTransform->Get_WorldPos());
	vDir.Normalize();

	vStartPosition = m_pTransform->Get_WorldPos();
	vTargetPosition = m_vTargetPos + 50.f * vDir;

	auto pLaser0 = Get_Component<CObjectContainer>()->Find_ObjectByName("Laser0");
	auto pLaser1 = Get_Component<CObjectContainer>()->Find_ObjectByName("Laser1");

	if (pLaser0 && pLaser1)
	{
		auto& laser0Context = static_cast<CEffectContainer*>(pLaser0)->GetEffectContext();
		auto& laser1Context = static_cast<CEffectContainer*>(pLaser1)->GetEffectContext();

		laser0Context.vLinePoint0 = vStartPosition;
		laser0Context.vLinePoint1 = vTargetPosition;

		laser1Context.vLinePoint0 = vStartPosition;
		laser1Context.vLinePoint1 = vTargetPosition;
	}

	m_fElaspedTime += dt;
	if (m_fElaspedTime >= m_fDuration)
	{
		if (pLaser1)
		{
			if (m_IsFlikering)
			{
				static_cast<CEffectContainer*>(pLaser1)->Play();
				m_IsFlikering = false;
			}
			else
			{
				static_cast<CEffectContainer*>(pLaser1)->Stop();
				m_IsFlikering = true;
			}
			
		}

		m_fElaspedTime = 0.f;
	}
}

CJaegerLaser* CJaegerLaser::Create()
{
	CJaegerLaser* instance = new CJaegerLaser();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CJaegerLaser");
	}

	return instance;
}

CGameObject* CJaegerLaser::Clone(INIT_DESC* pArg)
{
	CJaegerLaser* instance = new CJaegerLaser(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CJaegerLaser");
	}

	return instance;
}

void CJaegerLaser::Free()
{
	__super::Free();
}
