#pragma once
#include "DynamicBone.h"
NS_BEGIN(ModelEdit)

class CAIDynamicBone :
    public CDynamicBone
{

    struct DynamicBoneGuiState
    {
        int selectedGroupIndex = -1;
        int selectedChainIndex = -1;
        int selectedBoneIndex = -1;

        string boneName;   
        string boneFilter;

        bool showOnlyChildrenOfGroupRoot = true;
    };

protected:
    CAIDynamicBone();
    virtual ~CAIDynamicBone() DEFAULT;

public:
    HRESULT Initialize(class CAISkeleton* pSkeleton);

public:
    void Render_GUI();
    HRESULT Create_Chain(_int RootIndex);
    void Create_Node(vector<_int> Indices, DYNAMIC_CHAIN_GROUP& ChineGroup);

public: 
    void Save_File(ofstream& ofs);

private:
    _bool FilterBone(const string& name);

private:
    void RemoveGroup(int groupIndex);
    void RemoveChain(int groupIndex, int chainIndex);

private:
    class CAISkeleton* m_pSkeleton = { nullptr };
    vector<_float4x4> m_TPose;
    DynamicBoneGuiState m_Gui;
public:
    static CAIDynamicBone* Create(class CAISkeleton* pSkeleton);
    virtual void Free() override;
};

NS_END
