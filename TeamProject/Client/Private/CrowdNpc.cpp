#include "pch.h"
#include "CrowdNpc.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "Helper_Func.h"

static vector<_float3> dstPoint = 
{
	{-1.5f,0.f,45.f},{-2.f,0.f,-4.f},{-24.f,0.f,-3.5f},{-29.f,0.f,-1.f},{-30.f,0.f,2.f},
	{30.f,0.f,-30.f},
};

CCrowdNpc::CCrowdNpc()
{
}

CCrowdNpc::CCrowdNpc(const CCrowdNpc& rhs)
	:CNpc(rhs)
{
}

HRESULT CCrowdNpc::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CSkeletalModel>();
	Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CCrowdNpc::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	Get_Component<CModel>()->Link_Model(G_GlobalLevelKey, "JaneDoeModel.model");
	Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "JaneDoe.mat");

	m_NowPos = dstPoint[0];
	m_CurPointIdx = 0;
	m_pTransform->Set_Pos(m_NowPos);
	return S_OK;
}

void CCrowdNpc::Awake()
{
}

void CCrowdNpc::Priority_Update(_float dt)
{
}

void CCrowdNpc::Update(_float dt)
{
	Calc_Destination(dt);
}

void CCrowdNpc::Late_Update(_float dt)
{
}

void CCrowdNpc::Calc_Destination(_float dt)
{
	if (dstPoint.size() < 2) return;

	_uint lastIndex = dstPoint.size() - 1;
	if (m_CurPointIdx >= lastIndex) m_CurPointIdx = 0;

	_vector3 nowPos = m_pTransform->Get_WorldPos();
	_vector3 dstPos = dstPoint[m_CurPointIdx + 1];

	_vector3 toTarget = dstPos - nowPos;
	toTarget.y = 0.f;

	_float dist = toTarget.Length();
	if (dist < 0.2f){
		m_CurPointIdx++;
		return;
	}

	toTarget.Normalize();

	_vector3 moveDelta = toTarget * 3.f * dt;
	m_pTransform->Translate(moveDelta);

	_float targetYaw = atan2f(toTarget.x, toTarget.z);	
	_float currentYaw = m_CurYaw;       

	float newYaw = Math::Lerp(currentYaw, targetYaw, 0.12f, dt);
	m_pTransform->Rotate({0,newYaw ,0});
	m_CurYaw = newYaw;
}

CCrowdNpc* CCrowdNpc::Create()
{
	CCrowdNpc* instance = new CCrowdNpc();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CCrowdNpc");
	}

	return instance;
}

CGameObject* CCrowdNpc::Clone(INIT_DESC* pArg)
{
	CCrowdNpc* pInstance = new CCrowdNpc(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CCrowdNpc::Free()
{
	__super::Free();
}

