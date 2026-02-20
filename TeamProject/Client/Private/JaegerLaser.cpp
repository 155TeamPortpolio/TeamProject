#include "pch.h"
#include "JaegerLaser.h"
#include "EffectContainer.h"

// Component
#include "ObjectContainer.h"

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
	return S_OK;
}

HRESULT CJaegerLaser::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	Initialize_Effects();

	m_isAlive = false;

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
	m_fElaspedTime += dt;
	if (m_fElaspedTime >= m_fDuration)
	{
		auto pLaser1 = Get_Component<CObjectContainer>()->Find_ObjectByName("Laser1");
		if (pLaser1)
			static_cast<CEffectContainer*>(pLaser1)->Play();

		m_fElaspedTime = 0.f;
	}

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

	auto pObjectContainer = Get_Component<CObjectContainer>();

	auto pLaser0 = pObjectContainer->Find_ObjectByName("Laser0");
	if (pLaser0)
		static_cast<CEffectContainer*>(pLaser0)->Play();

	m_fElaspedTime = 0.f;
}

void CJaegerLaser::Deactive_Laser()
{
	auto pObjectContainer = Get_Component<CObjectContainer>();

	auto pLaser0 = pObjectContainer->Find_ObjectByName("Laser0");
	if (pLaser0)
		static_cast<CEffectContainer*>(pLaser0)->Stop();

	auto pLaser1 = pObjectContainer->Find_ObjectByName("Laser1");
	if (pLaser1)
		static_cast<CEffectContainer*>(pLaser0)->Stop();
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
