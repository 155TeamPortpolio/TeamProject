#include "pch.h"
#include "BackgroundNpc.h"
#include "SkeletalModel.h"
#include "Material.h"

#include "ObjectContainer.h"
#include "GameInstance.h"
#include "PedestrianNpc.h"
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

	PrototypeManager()->Add_ProtoType("MainCity_Level","Proto_GameObject_Pedestrian", CPedestrianNpc::Create());
	PrototypeManager()->Add_ProtoType("MainCity_Level","Proto_GameObject_Crowd", CCrowdNpc::Create());
	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CBackgroundNpc::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	//Build_Pedestrian(3);
	Build_Crowd(3,true);
	Build_Crowd(2);
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

void CBackgroundNpc::Build_Crowd(_uint Count, _bool Round)
{
    const _uint count = (Count == 0) ? 1u : Count;
    const _float radius = 0.6f;
    const _float arc = XM_PI * 1.6f;
    const _float angleStep = (count <= 1) ? 0.f : (arc / static_cast<_float>(count - 1));
    auto* transform = Get_Component<CTransform>(); 
    _vector3 look = transform ? (_vector3)transform->Dir(STATE::LOOK) : _vector3(0.f, 0.f, 1.f);

    look.y = 0.f;
    if (look.LengthSquared() < 1e-8f)
        look = _vector3(0.f, 0.f, 1.f);
    look.Normalize();

    _vector3 axisDir = -look;
    axisDir.Normalize();

    const _vector3 center(0.f, 0.f, 0.f);

    for (_uint idx = 0; idx < count; ++idx)
    {
        const _float angleOffset = -arc * 0.5f + angleStep * static_cast<_float>(idx);

        const _float c = cosf(angleOffset);
        const _float s = sinf(angleOffset);

        _vector3 dir;
        dir.x = axisDir.x * c - axisDir.z * s;
        dir.y = 0.f;
        dir.z = axisDir.x * s + axisDir.z * c;
        dir.Normalize();

        _vector3 pos = center + dir * radius;

        _vector3 toCenter = center - pos;
        toCenter.y = 0.f;
        if (toCenter.LengthSquared() < 1e-8f)
            toCenter = _vector3(0.f, 0.f, 1.f);
        toCenter.Normalize();

        _float yaw = atan2f(toCenter.x, toCenter.z);

		if (Round) {
			auto child = Builder::Create_Object({ "MainCity_Level", "Proto_GameObject_Crowd" })
				.Position({ pos.x, 0.f, pos.z })
				.Rotate({ 0.f, yaw, 0.f })
				.Build("Crowd_R_" + to_string(idx));

			Get_Component<CObjectContainer>()->Add_Child(child, true);
		}
		else {
			auto child = Builder::Create_Object({ "MainCity_Level", "Proto_GameObject_Crowd" })
				.Position({ idx *radius, 0.f, pos.z })
				.Build("Crowd_L" + to_string(idx));

			Get_Component<CObjectContainer>()->Add_Child(child, true);
		}
    }
}

void CBackgroundNpc::Build_Pedestrian(_uint Count)
{
	_uint count = Count;
	for (_uint idx = 0; idx < count; ++idx)
	{
		auto child = Builder::Create_Object({ "MainCity_Level", "Proto_GameObject_Pedestrian" }).
			Build("Pedestrian_"+to_string(idx));
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
