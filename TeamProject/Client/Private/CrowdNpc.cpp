#include "pch.h"
#include "CrowdNpc.h"

#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "GameInstance.h"
#include "PipeLine.h"

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

    auto Desc = static_cast<CrowdNpcDesc*>(pArg);
    m_strSpeech = Desc->Text;

    return S_OK;
}

void CCrowdNpc::Awake()
{
    modelPreset.RandomizeModel(false, Get_Component<CSkeletalModel>(), Get_Component<CMaterial>(), Get_Component<CAnimator3D>());
    colorPreset.Randomize_Natural(m_ObjectID);
    colorPreset.LinkMaterial(Get_Component<CMaterial>());
}

void CCrowdNpc::Priority_Update(_float dt)
{
  auto pPipeLine = RenderSystem()->Get_Pipeline();
  if (pPipeLine) {
      auto pModel = Get_Component<CModel>();
      auto box = pModel->Get_LocalBoundingBox();
     _bool isVisible =  pPipeLine->isVisible(pModel->Get_LocalBoundingBox(), _smatrix(m_pTransform->Get_WorldMatrix()));
     if (isVisible) {
         m_eRenderLayer = RENDER_LAYER::Default;
     }
     else {
         m_eRenderLayer = RENDER_LAYER::None;
     }
  }
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
    colorPreset.Render_Colors();
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
