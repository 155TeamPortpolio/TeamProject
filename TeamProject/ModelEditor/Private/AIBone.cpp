#include "pch.h"
#include "AiBone.h"

CAIBone::CAIBone()
{
}

HRESULT CAIBone::Initialize(const aiNode* _pAINode, _int _iParentIndex)
{
    m_iParentBoneIndex = _iParentIndex;
    m_BoneName = _pAINode->mName.C_Str();
    memcpy(&m_TransformationMatrix, &_pAINode->mTransformation, sizeof(_float4x4));
    XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
    return S_OK;
}

CAIBone* CAIBone::Create(const aiNode* _pAINode, _int _iParentIndex)
{
    CAIBone* pBone = new CAIBone();

    if (FAILED(pBone->Initialize(_pAINode, _iParentIndex))) {
        MSG_BOX("Create Failed : Engine | CAIBone");
        return nullptr;
    }

    return pBone;
}

void CAIBone::Free()
{
    __super::Free();
}
