#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class ENGINE_DLL CModelData :
	public CBase
{
protected:
	CModelData();
	virtual ~CModelData() override;

public:
	HRESULT Initialize(const string& filePath, ID3D11Device* pDevice);
#pragma region Rendering
	HRESULT Render_Mesh(ID3D11DeviceContext* pContext, _uint Index);

#pragma endregion 

#pragma region Mesh
	_uint Get_MeshCount() { return m_Meshes.size(); }
	_uint Get_MaterialIndex(_uint meshIndex);
	_bool hasMesh() { return !m_Meshes.empty(); }
	class CMesh* Get_Mesh(_uint meshIndex) { return m_Meshes[meshIndex]; }
	const D3D11_INPUT_ELEMENT_DESC* Get_ElementDesc(_uint DrawIndex);
	const _uint Get_ElementCount(_uint DrawIndex);
	const string_view Get_ElementKey(_uint DrawIndex);
	_bool isSkinned() { return (m_pSkeleton != nullptr); }
	_int Find_MeshIndex(const string& name);
#pragma endregion 

#pragma region Skeleton
	_uint Get_BoneCount();
	_int Find_BoneIndexByName(const string& BoneName);
	const string& Find_BoneNameByIndex(_uint BoneIndex);
	_bool isRootBone(_uint BoneIndex);
	_int Get_BoneParentIndex(_uint i);
	_float4x4 Get_TransformMatrix(_uint BoneIndex);
	_matrix Get_OffsetMatrix(_uint BoneIndex);

	const vector<string> Get_BoneNames();
	const vector<_int> GenerateFollowingIndices(class CModelData* pMasterData);
	MINMAX_BOX Get_LocalBoundingBox();
	MINMAX_BOX Get_MeshBoundingBox(_uint index);

#pragma endregion 
	virtual void Render_GUI();
#pragma region GUI

protected:
	class CSkeleton* m_pSkeleton = { nullptr };
	vector<class CMesh*> m_Meshes;
	
	_float3 m_vMinLocal = { FLT_MAX ,FLT_MAX ,FLT_MAX };
	_float3 m_vMaxLocal = { -FLT_MAX ,-FLT_MAX ,-FLT_MAX };

	_bool isGui_BoneTabOpen = { false };
public:
	static CModelData* Create(const string& filePath, ID3D11Device* pDevice);
	virtual void Free() override;
};

NS_END