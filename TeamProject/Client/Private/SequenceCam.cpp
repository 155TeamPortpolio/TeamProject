#include "pch.h"
#include "SequenceCam.h"
#include "GameInstance.h"

HRESULT CSequenceCam::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    m_seqPlayer = Add_Component<CCamSequencePlayer>();
    return S_OK;
}

HRESULT CSequenceCam::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
    return S_OK;
}

CSequenceCam* CSequenceCam::Create()
{
    auto inst = new CSequenceCam();
    if (FAILED(inst->Initialize_Prototype()))
    {
        MSG_BOX("Object Create Failed : CSequenceCam");
        Safe_Release(inst);
    }
    return inst;
}

CGameObject* CSequenceCam::Clone(INIT_DESC* pArg)
{
    auto inst = new CSequenceCam(*this);
    if (FAILED(inst->Initialize(pArg)))
    {
        MSG_BOX("Object Clone Failed : CSequenceCam");
        Safe_Release(inst);
    }
    return inst;
}