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
	vector<_uint> Get_LOD0_Index() { return m_LOD0Marked;  }
	vector<_uint> Get_LOD1_Index() { return m_LOD1Marked;  }
	vector<_uint> Get_LOD2_Index() { return m_LOD2Marked;  }
	vector<_uint> Get_LOD3_Index() { return m_LOD3Marked;  }
	vector<_uint> Get_Eff_Index()  { return m_EffMarked;   }

	vector<_uint> Get_MeshIndex_WithOutProxy();
	class CAIMesh* Get_AIMesh(_uint index);
private:
	vector<_uint> m_NotProxy;
	vector<_uint> m_ProxyMarked;
	vector<_uint> m_LOD0Marked;
	vector<_uint> m_LOD1Marked;
	vector<_uint> m_LOD2Marked;
	vector<_uint> m_LOD3Marked;
	vector<_uint> m_EffMarked;
	vector<CAIMesh*> m_AIMesh;
public:
	static CModelData* Create(MESH_TYPE _eType, const aiScene* pAIScene);
	virtual void Free() override;
};

NS_END