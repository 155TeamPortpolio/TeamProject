#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)
struct MaterialUsageRow
{
    _uint materialIndex = 0;
    _uint meshCount = 0;
    vector<_uint> meshIndices;   
};

struct MeshSection
{
    _uint materialId;
    _uint indexStart;
    _uint indexCount;
};
struct MeshData
{
    vector<_uint> indices;        
    vector<MeshSection> sections;
};

struct MergedSection
{
    _uint materialId;
    _uint mergedIndexStart;
    _uint mergedIndexCount;
};

struct MeshMergedResult
{
    vector<_uint> mergedIndices;      
    vector<MergedSection> mergedSections;
};

static MeshMergedResult BuildMergedSections_ByMaterial(const MeshData& meshData)
{
    MeshMergedResult result;

    unordered_map<_uint, vector<const MeshSection*>> groups;
    groups.reserve(meshData.sections.size());

    for (const MeshSection& section : meshData.sections)
    {
        groups[section.materialId].push_back(&section);
    }

    result.mergedIndices.clear();
    result.mergedSections.clear();

    result.mergedIndices.reserve(meshData.indices.size());
    result.mergedSections.reserve(groups.size());

    for (auto& groupPair : groups)
    {
        const _uint materialId = groupPair.first;
        vector<const MeshSection*>& groupSections = groupPair.second;

        // indexStart 기준 정렬(캐시/연속성)
        sort(groupSections.begin(), groupSections.end(),
            [](const MeshSection* leftSection, const MeshSection* rightSection)
            {
                return leftSection->indexStart < rightSection->indexStart;
            });

        MergedSection mergedSection;
        mergedSection.materialId = materialId;
        mergedSection.mergedIndexStart = (_uint)result.mergedIndices.size();
        mergedSection.mergedIndexCount = 0u;

        for (const MeshSection* sectionPtr : groupSections)
        {
            const _uint sourceStart = sectionPtr->indexStart;
            const _uint sourceCount = sectionPtr->indexCount;

            for (_uint offsetIndex = 0u; offsetIndex < sourceCount; ++offsetIndex)
            {
                result.mergedIndices.push_back(meshData.indices[sourceStart + offsetIndex]);
            }

            mergedSection.mergedIndexCount += sourceCount;
        }

        result.mergedSections.push_back(mergedSection);
    }

    return result;
}

NS_END