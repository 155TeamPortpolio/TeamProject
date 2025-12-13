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

void CAIBone::Save_File(ofstream& ofs)
{
    BONE_INFO_HEADER infoHeader = {};
    strcpy_s(infoHeader.BoneName, sizeof(infoHeader.BoneName), m_BoneName.data());

    _matrix SaveMatrix = XMLoadFloat4x4(&m_TransformationMatrix);

    if (m_iParentBoneIndex == -1) {
        _matrix		PreTransformMatrix = XMMatrixIdentity();
        PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180));
        SaveMatrix = PreTransformMatrix * SaveMatrix;
    }
    XMStoreFloat4x4(&infoHeader.TransformationMatrix, SaveMatrix);
    infoHeader.ParentBoneIndex = { m_iParentBoneIndex };
    ofs.write(reinterpret_cast<const char*>(&infoHeader), sizeof(BONE_INFO_HEADER));
}

void CAIBone::Free()
{
    __super::Free();
}
