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
	void Save_File(ofstream& ofs);
private:

public:
	static CModelData* Create(MESH_TYPE _eType, const aiScene* pAIScene);
	virtual void Free() override;
};

NS_END