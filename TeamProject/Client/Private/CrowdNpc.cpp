#include "pch.h"
#include "CrowdNpc.h"

#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"

CCrowdNpc::CCrowdNpc()
{
}

CCrowdNpc::CCrowdNpc(const CCrowdNpc& rhs)
	: CNpc(rhs)
{
}

HRESULT CCrowdNpc::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CSkeletalModel>();
    Add_Component<CMaterial>();
    Add_Component<CAnimator3D>();
    return S_OK;
}

HRESULT CCrowdNpc::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Get_Component<CModel>()->Link_Model(G_GlobalLevelKey, "JaneDoeModel.model");
    Get_Component<CMaterial>()->Link_Material(G_GlobalLevelKey, "JaneDoe.mat");

    Get_Component<CAnimator3D>()->LinkAnimate_Model(G_GlobalLevelKey, "JaneDoeModel.model");
    Get_Component<CAnimator3D>()->Link_MetaData(G_GlobalLevelKey, "JaneDoe_Meta.json");
    Get_Component<CAnimator3D>()->Set_Animation("JaneDoe_Ani_Idle").Loop(true).Apply();
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
    Get_Component<CAnimator3D>()->Update_Animation(dt);
}

void CCrowdNpc::Late_Update(_float dt)
{
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
    CCrowdNpc* instance = new CCrowdNpc(*this);
    if (FAILED(instance->Initialize(pArg)))
    {
        Safe_Release(instance);
        return nullptr;
    }
    return instance;
}

void CCrowdNpc::Free()
{
    __super::Free();
}
