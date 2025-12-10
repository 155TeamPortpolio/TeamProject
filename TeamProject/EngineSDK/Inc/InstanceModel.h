#pragma once
#include "Model.h"

NS_BEGIN(Engine)

class ENGINE_DLL CInstanceModel 
	: public CModel
{
public:

protected:
	CInstanceModel();
	CInstanceModel(const CInstanceModel& rhs);
	virtual ~CInstanceModel();

public:
	HRESULT Initialize(COMPONENT_DESC* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Link_Model(const string& levelKey, const string& modelDataKey) override;
	virtual HRESULT Link_InstanceData(ID3D11Device* pDevice , vector<INSTANCE_INIT_DESC> initArray, const string& LevelKey, const string& ModelKey);
	void	Link_InstanceWithMesh(_uint MeshIndex, _uint InstanceIndex);
	void	Link_InstanceMeshAll( _uint InstanceIndex);

public:
	HRESULT Update_Instance(ID3D11DeviceContext* pContext, const void* pData, _uint Index, _uint count);
	HRESULT Bind_Buffer(ID3D11DeviceContext* pContext, _uint MeshIndex);
	virtual HRESULT Draw(ID3D11DeviceContext* pContext, _uint MeshIndex);

public:
	virtual const D3D11_INPUT_ELEMENT_DESC* Get_ElementDesc(_uint meshIndex) override;
	virtual const _uint Get_ElementCount(_uint meshIndex) override;
	virtual const string_view Get_ElementKey(_uint meshIndex) override;

public:
	virtual _uint Get_MeshCount() override;
	virtual _uint Get_MaterialIndex(_uint Index) override;
	virtual _bool isDrawable(_uint Index) override;
	virtual void SetDrawable(_uint Index, _bool isDraw) override;
	_bool isReadyToDraw()	override;

public:
	virtual MINMAX_BOX Get_LocalBoundingBox()	override;
	virtual vector<MINMAX_BOX> Get_MeshBoundingBox()	override;
	virtual MINMAX_BOX Get_WorldBoundingBox()	override;
	virtual MINMAX_BOX Get_MeshBoundingBox(_uint index)	override;

private:
	_bool Check_Valid(_uint MeshIndex);

private:
	class CModelData* m_pModelData = { nullptr };
	vector<_bool> m_DrawableMeshes;

	vector<ID3D11Buffer*>       m_InstanceBuffers;
	vector<INSTANCE_INIT_DESC>  m_DescArray;
	

	vector<_uint>					m_ActiveCounts;

	vector<string_view>      m_StringViews;
	vector<string>      m_CombinedViews;
	unordered_map<_uint, vector<D3D11_INPUT_ELEMENT_DESC> >m_CombinedDescs;
	unordered_map<_uint, _uint> m_MapMeshWithInstance;

public:
	static CInstanceModel* Create();
	virtual CComponent* Clone() override;
	virtual void Free() override;
};

NS_END