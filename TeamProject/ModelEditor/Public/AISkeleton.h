#pragma once
#include "ModelEditor_Defines.h"
#include "Assimps.h"
#include "Skeleton.h"

NS_BEGIN(ModelEdit)

class CAISkeleton final : public CSkeleton
{
private:
	CAISkeleton();
	virtual ~CAISkeleton() = default;

public:
	HRESULT Initialize(const aiNode* _pAINode);

public:
	//_bool Find_BoneIndex(const _char* pName, _uint* _iGetIndex = nullptr);
	//class CAIBone* Find_Bone(const _char* pName) const;
	void Set_Offset(_uint Index, _float4x4 offset);
	void Save_File(ofstream& ofs);

private:
	HRESULT Ready_Bones(const aiNode* _pAINode, _int _iParentIndex = -1);

public:
	static CAISkeleton* Create(const aiNode* _pAINode);
	virtual void Free() override;
};

NS_END