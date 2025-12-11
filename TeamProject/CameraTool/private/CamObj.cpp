#include "pch.h"

HRESULT CamObj::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CCamera>();
	Add_Component<CLight>();
	return S_OK;
}

HRESULT CamObj::Initialize(INIT_DESC* arg)
{
	__super::Initialize(arg);
	m_pTransform->LookAt({ 0, 0, 0 });
	return S_OK;
}

void CamObj::Priority_Update(_float dt)
{
}

void CamObj::Update(_float dt)
{
}

void CamObj::Late_Update(_float dt)
{
}

CamObj* CamObj::Create()
{
	auto inst = new CamObj();
	inst->Initialize_Prototype();
	return inst;
}

CGameObject* CamObj::Clone(INIT_DESC* arg)
{
	auto inst = new CamObj(*this);
	inst->Initialize(arg);
	return inst;
}

void CamObj::Free()
{
	__super::Free();
}