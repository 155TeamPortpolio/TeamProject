#pragma once
#include "ModelEditor_Defines.h"
#include "ModelData.h"
#include "Assimps.h"

NS_BEGIN(ModelEdit)
class CAIModelData :
	public CModelData
{
private:
	CAIModelData();
	virtual ~CAIModelData() DEFAULT;

public:
	HRESULT Initialize(MESH_TYPE _eType, const aiScene* pAIScene);
	void Save_File(ofstream& ofs, _fmatrix PreTransform);

public:
	vector<_uint> Get_ProxyIndex() { return m_ProxyMarked; }
	vector<_uint> Get_LOD1_Index() { return m_LOD1Marked; }
	vector<_uint> Get_LOD2_Index() { return m_LOD2Marked; }
	vector<_uint> Get_LOD3_Index() { return m_LOD3Marked; }

private:
	vector<_uint> m_ProxyMarked;
	vector<_uint> m_LOD1Marked;
	vector<_uint> m_LOD2Marked;
	vector<_uint> m_LOD3Marked;
public:
	static CModelData* Create(MESH_TYPE _eType, const aiScene* pAIScene);
	virtual void Free() override;
};

NS_END