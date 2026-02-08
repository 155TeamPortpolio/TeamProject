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
    //Add_Component<CAnimator3D>();
    return S_OK;
}
static int test= 1;
HRESULT CCrowdNpc::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    _int gender = Helper::Get_Random_Int(0, 1);
    _int variation = Helper::Get_Random_Int(1, 10);

    string gederWord = "Female";
    if (gender) {
        gederWord = "Female";
        variation = test++;//Helper::Get_Random_Int(1, 10);
    }

    string model = "NPC_Citizen_" + gederWord + "_" + std::to_string(variation) + ".model";
    string material = "NPC_Citizen_" + gederWord + "_" + std::to_string(variation) + ".mat";
    string meta = "NPC_" + gederWord;

    Get_Component<CModel>()->Link_Model("MainCity_Level", model);
    Get_Component<CMaterial>()->Link_Material("MainCity_Level", material);

    //Get_Component<CAnimator3D>()->LinkAnimate_Model("MainCity_Level", model);
    //Get_Component<CAnimator3D>()->Link_MetaData(G_GlobalLevelKey, meta + "_Size02_Meta.json");
    //Get_Component<CAnimator3D>()->Set_Animation("NPC_Male_Size02_Ani_MainCity_EchoPose05_Loop").Loop(true).Apply();

    m_InstanceName = model;
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
   // Get_Component<CAnimator3D>()->Update_Animation(dt);
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
