#pragma once
#include "Model.h"

NS_BEGIN(Engine)
class ENGINE_DLL CTrailModel :
    public CModel
{
public:
	enum class MODE { CENTER, SEGMENT, END };
private:
	CTrailModel();
	CTrailModel(const CTrailModel& rhs);
	virtual ~CTrailModel() DEFAULT;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(COMPONENT_DESC* pArg) override;
	virtual const D3D11_INPUT_ELEMENT_DESC* Get_ElementDesc(_uint DrawIndex)override;
	virtual const _uint Get_ElementCount(_uint DrawIndex)override;
	virtual const string_view Get_ElementKey(_uint DrawIndex)override;
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
	HRESULT Bind_Buffer(ID3D11DeviceContext* pContext);
	virtual HRESULT Draw(ID3D11DeviceContext* pContext, _uint Index)override;

public:
	void UpdateTrail(_float dt);

private:
	void BuildVertices();

	class CVI_Trail* m_pBuffer = { nullptr };
	_uint m_iAlivePointCount{};

public:
	static CTrailModel* Create();
	CComponent* Clone() override;
	virtual void Free() override;


};
NS_END
