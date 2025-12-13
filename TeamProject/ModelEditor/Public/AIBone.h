#pragma once
#include "ModelEditor_Defines.h"
#include "Assimps.h"
#include "Bone.h"

NS_BEGIN(ModelEdit)

class CAIBone final : public CBone
{
private:
	CAIBone();
	virtual ~CAIBone() DEFAULT;

public:
	void Get_BoneInfo(BONE_INFO_HEADER* OutBoneInfoHeader) const {
		memcpy(OutBoneInfoHeader->BoneName, &m_BoneName, MAX_PATH);
		OutBoneInfoHeader->ParentBoneIndex = m_iParentBoneIndex;
		memcpy(&OutBoneInfoHeader->TransformationMatrix, &m_TransformationMatrix, sizeof(_float4x4));
	}

	_bool Compare_Name(string NameTag) {
		return !strcmp(NameTag.c_str(), m_BoneName.c_str());
	}

public:
	HRESULT Initialize(const aiNode* _pAINode, _int _iParentIndex);

public:
	static CAIBone* Create(const aiNode* _pAINode, _int _iParentIndex);
	virtual void Free() override;
};

NS_END