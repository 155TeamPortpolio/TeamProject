#include "Engine_Defines.h"
#include "Skeleton.h"
#include "Bone.h"

CSkeleton::CSkeleton()
{
}

HRESULT CSkeleton::InitializeFromFile(ifstream& ifs)
{
    SKELETON_FILE_HEADER fileHeader = {};
    ifs.read(reinterpret_cast<char*>(&fileHeader), sizeof(SKELETON_FILE_HEADER));

    for (size_t i = 0; i < fileHeader.BoneCount; i++)
    {
        CBone* pBone = CBone::Create(ifs);
        m_Bones.push_back(pBone);
        m_BoneMap.emplace(pBone->Get_Name(), i);
    }

    for (size_t i = 0; i < fileHeader.BoneCount; i++)
    {
        _float4x4 LoadFloat4x4 = {};
        ifs.read(reinterpret_cast<char*>(&LoadFloat4x4), sizeof(_float4x4));
        m_OffsetMatrices.push_back(LoadFloat4x4);
    }



    return S_OK;
}

_uint CSkeleton::Get_BoneParentIndex(_uint i)
{
    return m_Bones[i]->Get_ParentIndex();
}

_float4x4 CSkeleton::Get_TransformationMatrix(_uint BoneIndex)
{
     return m_Bones[BoneIndex]->Get_TransformationMatrix();
}

const vector<string> CSkeleton::Get_BoneNames()
{
    vector<string> names; 

    for (size_t i = 0; i < m_Bones.size(); i++)
    {
        names.push_back(m_Bones[i]->Get_Name());
    }

    return names;
}

_int CSkeleton::Find_BoneIndexByName(const string& boneName)
{
    auto iter = m_BoneMap.find(boneName);

    if (iter != m_BoneMap.end())
        return iter->second;
    else {
        return FindBoneIndexWithPrefix(boneName);
    }
}

const string& CSkeleton::Find_BoneNameByIndex(_uint boneIndex)
{
    return m_Bones[boneIndex]->Get_Name();
}

_int CSkeleton::FindBoneIndexWithPrefix(const string& BonePrefixName)
{
    string prefix = "Armature_" + BonePrefixName;
    auto iter = m_BoneMap.find(prefix);

    if (iter != m_BoneMap.end())
        return iter->second;
    else {
        return -1;
    }
}
void CSkeleton::Render_GUI()
{
    for (size_t i = 0; i < m_Bones.size(); i++)
    {
        string name = "("+to_string(i) + ")" + m_Bones[i]->Get_Name();
        ImGui::Text(name.c_str());
        if (ImGui::IsItemHovered())
        {
            _int parentIndex = m_Bones[i]->Get_ParentIndex();

            if (parentIndex != -1) {
                CBone* pParent = m_Bones[parentIndex];
                ImGui::SetTooltip("Parent: %s", pParent->Get_Name().c_str());
            }
            else
                ImGui::SetTooltip("Parent: <None>");
        }
    }
}

CSkeleton* CSkeleton::Create(ifstream& ifs)
{
    CSkeleton* instance = new CSkeleton;
    if (FAILED(instance->InitializeFromFile(ifs))) {
        Safe_Release(instance);
    }
    return instance;
}

void CSkeleton::Free()
{
    __super::Free();

    for (auto& bones : m_Bones)
        Safe_Release(bones);

    m_BoneMap.clear();
    m_Bones.clear();
}
