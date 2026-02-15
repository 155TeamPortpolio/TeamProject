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

    _int isMale = Helper::Get_Random_Int(0, 1);
    _int variation = Helper::Get_Random_Int(1, 10);

    string gederWord = "Female";
    if (isMale) {
        gederWord = "Male";
        variation = Helper::Get_Random_Int(1, 6);
    }
    else {
        variation = Helper::Get_Random_Int(1, 2);
    }

    string model =      gederWord + "0" + to_string(variation) + ".model";
    string material =   gederWord + "0" + to_string(variation) + ".mat";
    string meta = gederWord;

    Get_Component<CModel>()->Link_Model("MainCity_Level", model);
    Get_Component<CMaterial>()->Link_Material("MainCity_Level", material);

    Get_Component<CAnimator3D>()->LinkAnimate_Model("MainCity_Level", model);
    if (isMale) {
        Get_Component<CAnimator3D>()->Link_MetaData("MainCity_Level", "NPC_Male_Size02_Meta.json");
    }
    else {
        Get_Component<CAnimator3D>()->Link_MetaData("MainCity_Level", "NPC_Female_Size02_Meta.json");
    }
    
    Get_Component<CAnimator3D>()->Set_Animation(4).Loop(true).Apply();
    //NPC_Male_Size02_Meta.json
    //NPC_Female_Size02_Meta.json
    m_InstanceName = model;
    return S_OK;
}

void CCrowdNpc::Awake()
{
    if (auto pHairMat = Get_Component<CMaterial>()->Find_MaterialInstanceByName("Hair")) {
        pHairMat->Set_Param("fVariationColor", {&m_fHairColor ,"float4",sizeof(_float4)});
    }
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

void CCrowdNpc::Render_GUI()
{
    ImGui::DragFloat4("Hair", &m_fHairColor.x);
    __super::Render_GUI();
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
