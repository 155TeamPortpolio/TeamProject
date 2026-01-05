#pragma once
#include "ModelEditor_Defines.h"
#include "Skeleton.h"
#include "ModelEditor_BoneData.h"

NS_BEGIN(ModelEdit)

class CAISkeleton final : public CSkeleton
{
private:
	CAISkeleton();
	virtual ~CAISkeleton() = default;

public:
	HRESULT Initialize(const aiNode* _pAINode);

public:
	void Set_Offset(_uint Index, _float4x4 offset);
	void Save_File(ofstream& ofs, _fmatrix PreTransform);
	void Rake_BoneInfo(BONE_DATA_HEADER* pHeader);

public:
	void Render_GUI();
private:
	HRESULT Ready_Bones(const aiNode* _pAINode, _int _iParentIndex = -1);

private:
	vector<_bool> HasOffset;
	vector<_float4x4> ErroredOffset;
public:
	static CAISkeleton* Create(const aiNode* _pAINode);
	virtual void Free() override;
};

NS_END