#pragma once
#include "Base.h"

NS_BEGIN(Engine)
using MESHBONE = vector<class CBone*>;
class ENGINE_DLL CSkeleton :
    public CBase
{
protected:
    CSkeleton();
    virtual ~CSkeleton() DEFAULT;
public:
    HRESULT InitializeFromFile(ifstream& ifs );

public:
    _int Find_BoneIndexByName(const string& boneName);
    const string& Find_BoneNameByIndex(_uint boneIndex);
    _uint Get_BoneCount() { return m_Bones.size(); };
    _uint Get_BoneParentIndex(_uint i);
    _matrix Get_OffsetMatrix(_uint BoneIndex) { return XMLoadFloat4x4(&m_OffsetMatrices[BoneIndex]); };
    _float4x4 Get_TransformationMatrix(_uint BoneIndex);

    const vector<string> Get_BoneNames();

private:
    _int FindBoneIndexWithPrefix(const string& BonePrefixName);

public:
    virtual void Render_GUI();

protected:
    vector<class CBone*> m_Bones;
    vector<_float4x4> m_OffsetMatrices;
    unordered_map<string, _uint> m_BoneMap;//이름 검색용 컨테이너

public:
    static CSkeleton* Create(ifstream& ifs);
    virtual void Free() override;
};

NS_END