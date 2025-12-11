#include "Engine_Defines.h"
#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::InitializeFromFile(ifstream& ifs)
{
    BONE_INFO_HEADER infoHeader = {};
    ifs.read(reinterpret_cast<char*>(&infoHeader), sizeof(BONE_INFO_HEADER));

    m_BoneName = infoHeader.BoneName;
    m_iParentBoneIndex = infoHeader.ParentBoneIndex;
    m_TransformationMatrix = infoHeader.TransformationMatrix;

	return S_OK;
}

void CBone::Render_GUI()
{
}

CBone* CBone::Create(ifstream& ifs)
{
    CBone* instance = new CBone;
    if (FAILED(instance->InitializeFromFile(ifs))) {
        Safe_Release(instance);
    }
    return instance;
}

void CBone::Free()
{
}
