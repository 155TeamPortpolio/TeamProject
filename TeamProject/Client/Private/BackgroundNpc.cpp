#include "pch.h"
#include "BackgroundNpc.h"
#include "SkeletalModel.h"
#include "Material.h"

#include "ObjectContainer.h"
#include "GameInstance.h"
#include "CrowdNpc.h"

CBackgroundNpc::CBackgroundNpc()
{
}

CBackgroundNpc::CBackgroundNpc(const CBackgroundNpc& rhs)
	:CNpc(rhs)
{
}

HRESULT CBackgroundNpc::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	PrototypeManager()->Add_ProtoType("Test_Level","Proto_Crowd", CCrowdNpc::Create());
	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CBackgroundNpc::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	Build_Pedestrian(3);

	return S_OK;
}

void CBackgroundNpc::Awake()
{
	
}

void CBackgroundNpc::Priority_Update(_float dt)
{
	Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}

void CBackgroundNpc::Update(_float dt)
{
	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CBackgroundNpc::Late_Update(_float dt)
{
	Get_Component<CObjectContainer>()->Late_UpdateChild(dt);
}

void CBackgroundNpc::Build_Crowd(_uint Count)
{
	_uint count = Count;
	_float radius = 0.6f;
	_float angleStep = (XM_PI*.8f) / static_cast<_float>(count);

	for (_uint idx = 0; idx < count; ++idx)
	{
		_float angle = angleStep * static_cast<_float>(idx);
		_float x = cosf(angle) * radius;
		_float z = sinf(angle) * radius;
		_vector3 toCenter = _vector3(0.f, 0.f, 0.f) - _vector3(x, 0.f, z);

		if (toCenter.LengthSquared() < 1e-8f)
			toCenter = _vector3(0.f, 0.f, 1.f);

		toCenter.Normalize();
		_float yaw = atan2f(toCenter.x, toCenter.z);

		auto child = Builder::Create_Object({ "Test_Level", "Proto_Crowd" })
			.Position({ x, 0.f, z })
			.Rotate({ 0.f, yaw, 0.f })
			.Build(std::to_string(idx));

		Get_Component<CObjectContainer>()->Add_Child(child, true);
	}
}

void CBackgroundNpc::Build_Pedestrian(_uint Count)
{
	_uint count = Count;
	for (_uint idx = 0; idx < count; ++idx)
	{
		auto child = Builder::Create_Object({ "Test_Level", "Proto_Crowd" }).Build(std::to_string(idx));
		Get_Component<CObjectContainer>()->Add_Child(child, false);
	}
}

CBackgroundNpc* CBackgroundNpc::Create()
{
	CBackgroundNpc* instance = new CBackgroundNpc();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CBackgroundNpc");
	}

	return instance;
}

CGameObject* CBackgroundNpc::Clone(INIT_DESC* pArg)
{
	CBackgroundNpc* pInstance = new CBackgroundNpc(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CBackgroundNpc::Free()
{
	__super::Free();
}
