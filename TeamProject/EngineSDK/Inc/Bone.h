#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class ENGINE_DLL CBone :
    public CBase
{
protected:
    CBone();
    virtual ~CBone() DEFAULT;
public:
    HRESULT InitializeFromFile(ifstream& ifs);
    const string& Get_Name()    { return m_BoneName; }
    _int Get_ParentIndex()           {return m_iParentBoneIndex; };
    _float4x4 Get_TransformationMatrix()           {return m_TransformationMatrix; };
public:
    virtual void Render_GUI();
protected:
    string				m_BoneName = {};
    _float4x4			m_TransformationMatrix = {};
    _int				m_iParentBoneIndex = { -1 };

public:
    static CBone* Create(ifstream& ifs);
    virtual void Free() override;
};

NS_END