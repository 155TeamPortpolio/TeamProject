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
	void Render_Humanoid();

private:
	HRESULT Ready_Bones(const aiNode* _pAINode, _int _iParentIndex = -1);

private:
	const string SlotName(HumanoidBone boneType);
	_bool IsRequiredSlot(HumanoidBone slot);
	_bool CheckAncester(_int ancestorIndex, _int childIndex);

private:
	vector<_bool> HasOffset;

private: /*Humanoid*/
	_bool m_isHumanoidTabOpened = { false };
	_int m_SelectedBoneIndex = {-1};
	_int m_SelectedSlotIndex = {-1};
	HumanoidRigData m_RiggedData = {};
	string m_BoneFilter = {};

public:
	static CAISkeleton* Create(const aiNode* _pAINode);
	virtual void Free() override;
};

NS_END