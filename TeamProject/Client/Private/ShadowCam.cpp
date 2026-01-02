#include "pch.h"
#include "ShadowCam.h"

CShadowCam::CShadowCam()
{
}

CShadowCam::CShadowCam(const CShadowCam& rhs)
	:CCamObject(rhs)
{
}

HRESULT CShadowCam::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	return S_OK;
}

HRESULT CShadowCam::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	return S_OK;
}

void CShadowCam::Awake()
{
	auto camera = Get_Component<CTransform>();
	camera->LookAt(XMVectorSet(0.f, 0.f, 0.f, 1.f));
}

void CShadowCam::Priority_Update(_float dt)
{
}

void CShadowCam::Update(_float dt)
{
}

void CShadowCam::Late_Update(_float dt)
{
}

CShadowCam* CShadowCam::Create()
{
	CShadowCam* Instance = new CShadowCam();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CShadowCam");
		Safe_Release(Instance);
	}

	return Instance;
}

CGameObject* CShadowCam::Clone(INIT_DESC* pArg)
{
	CShadowCam* Instance = new CShadowCam(*this);

	if (FAILED(Instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CShadowCam");
		Safe_Release(Instance);
	}

	return Instance;
}

void CShadowCam::Free()
{
	__super::Free();
}
