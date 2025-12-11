#pragma once
#include "Model.h"
NS_BEGIN(Engine)
class ENGINE_DLL CPlaneModel :
    public CModel
{
protected:
	CPlaneModel();
	CPlaneModel(const CPlaneModel& rhs);
	virtual ~CPlaneModel() DEFAULT;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(COMPONENT_DESC* pArg) override;
	virtual const D3D11_INPUT_ELEMENT_DESC* Get_ElementDesc(_uint DrawIndex)override;
	virtual const _uint Get_ElementCount(_uint DrawIndex)override;
	virtual const string_view Get_ElementKey(_uint DrawIndex)override;
	virtual HRESULT Draw(ID3D11DeviceContext* pContext, _uint Index)override;
	virtual HRESULT Link_Model(const string& levelKey, const string& modelDataKey)override;

public:
	_uint Get_MeshCount() override;
	_uint Get_MaterialIndex(_uint Index) override;
	_bool isDrawable(_uint Index) override;
	virtual void SetDrawable(_uint Index, _bool isDraw) override;
	virtual MINMAX_BOX Get_LocalBoundingBox()	override;
	virtual MINMAX_BOX Get_WorldBoundingBox()	override;
	virtual vector<MINMAX_BOX> Get_MeshBoundingBoxes()	override;
	virtual MINMAX_BOX Get_MeshBoundingBox(_uint index)	override;
	_bool isReadyToDraw()	override { return true; };

public:
	virtual void Render_GUI() override;

private:
	class CVIBuffer* m_pPlane = { nullptr };
	_bool isDrawing = { true };
public:
	static CPlaneModel* Create();
	CComponent* Clone() override;
	virtual void Free() override;
};

NS_END