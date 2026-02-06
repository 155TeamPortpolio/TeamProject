#include "pch.h"
#include "MiasmaBlade.h"
#include "StaticModel.h"
#include "Material.h"
#include "BattleSystem.h"
#include "Helper_Func.h"

CMiasmaBlade::CMiasmaBlade()
{
}

CMiasmaBlade::CMiasmaBlade(const CMiasmaBlade& rhs)
	:CEnemy(rhs)
{
}

HRESULT CMiasmaBlade::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CStaticModel>()->Link_Model(G_GlobalLevelKey, "Default.model");
	Add_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "Default.mat");
	return S_OK;
}

HRESULT CMiasmaBlade::Initialize(INIT_DESC* pArg)
{
	auto desc = static_cast<BladeDesc*>(pArg);
	__super::Initialize(desc);
	m_vTargetPos=desc->vTargetPos;
	m_pTransform->LookAt(_vector3(m_vTargetPos));
	return S_OK;
}

void CMiasmaBlade::Awake()
{
}

void CMiasmaBlade::Priority_Update(_float dt)
{
}

void CMiasmaBlade::Update(_float dt)
{
	m_pTransform->Translate(m_pTransform->Dir(STATE::LOOK)*15*dt);
}

void CMiasmaBlade::Late_Update(_float dt)
{
}
void CMiasmaBlade::Render_GUI()
{
    __super::Render_GUI();
}

void CMiasmaBlade::OnPooledAcquire(INIT_DESC* pArg)
{
}

void CMiasmaBlade::OnPooledRelease()
{
}

CMiasmaBlade* CMiasmaBlade::Create()
{
	CMiasmaBlade* instance = new CMiasmaBlade();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CDefiler");
	}

	return instance;
}

CGameObject* CMiasmaBlade::Clone(INIT_DESC* pArg)
{
	CMiasmaBlade* instance = new CMiasmaBlade(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CDefiler");
	}

	return instance;
}

void CMiasmaBlade::Free()
{
	__super::Free();
}

