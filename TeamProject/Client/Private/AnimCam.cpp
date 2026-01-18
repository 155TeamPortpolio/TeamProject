#include "pch.h"
#include "AnimCam.h"
// Components
#include "Animator3D.h"
#include "GameInstance.h"
HRESULT CAnimCam::Initialize_Prototype()
{
	__super::Initialize_Prototype();
    //Add_Component<CAnimator3D>();
	return S_OK;
}

HRESULT CAnimCam::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

    //Get_Component<CAnimator3D>()->LinkAnimate_Model(LevelManager()->Get_NowLevelKey(), "JaneDoeModel.model");
    //Get_Component<CAnimator3D>()->Link_MetaData(G_GlobalLevelKey, "JaneDoe_Meta.json");
    //Get_Component<CAnimator3D>()->Set_Animation("Avatar_Female_Size03_JaneDoe_Ani_JaneDoe_Idle")
    //    .Loop(true)
    //    .Apply();

	return S_OK;
}

void CAnimCam::Priority_Update(_float dt)
{

}

void CAnimCam::Update(_float dt)
{
}

void CAnimCam::Late_Update(_float dt)
{
    auto boneMat = handle.Get()->Get_Component<CAnimator3D>()->Get_BoneMatrix(CAnimator3D::BoneSpace::TRANSFORMATION, "bip001");

    m_pTransform->TranslateMatrix(Matrix(boneMat));
}

CAnimCam* CAnimCam::Create()
{
    auto inst = new CAnimCam();
    if (FAILED(inst->Initialize_Prototype()))
    {
        MSG_BOX("Object Create Failed : AnimCam");
        Safe_Release(inst);
    }
    return inst;
}

CGameObject* CAnimCam::Clone(INIT_DESC* pArg)
{
    auto inst = new CAnimCam(*this);
    if (FAILED(inst->Initialize(pArg)))
    {
        MSG_BOX("Object Clone Failed : AnimCam");
        Safe_Release(inst);
    }
    return inst;
}