#pragma once
#include "Model.h"

NS_BEGIN(Engine)
class ENGINE_DLL CTrailModel :
    public CModel
{
	typedef struct tagCenterPoint
	{
		_float3 vPosition{};
		_float fLifeTime{};
		_float fDistanceAcc{};
	}CENTER_POINT;

	typedef struct tagSegmentPoint
	{
		_float3 vPositionA{};
		_float3 vPositionB{};
		_float fLifeTime{};
		_float fDistanceAcc{};
	}SEGMENT_POINT;

public:
	enum class POINT_MODE { CENTER, SEGMENT, END };
	enum class TEXTURE_MODE { STRETCH, TILE, END };
	enum class COLOR_MODE : _uint { MULTIPLY = 0, ADDITIVE = 1, END = 2 };
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
	void SetTrailParams(TRAIL_NODE trailDesc);
	void Update_CenterPoint(_float3 position, _float dt);
	void Update_SegmentPoint(_float3 position0, _float3 position1, _float dt);

private:
	void BuildVertices();

	class CVI_Trail* m_pBuffer = { nullptr };

	POINT_MODE m_eMode = POINT_MODE::CENTER;
	TEXTURE_MODE m_eTextureMode = TEXTURE_MODE::STRETCH;
	COLOR_MODE m_eColorMode = COLOR_MODE::MULTIPLY;

	/* Texture Mode */
	_float2 m_vUVSpeed{};
	_float2 m_vUVOffset{};
	_float m_fTile{};

	/* Color Mode */
	_float4 m_vStartColor{};
	_float4 m_vEndColor{};

	_uint m_iAlivePointCount{};
	_float m_fMaxLifeTime{};
	_float m_fMinDistance{};
	vector<VTXTRAIL> m_TrailVertices;

	/* Center Mode */
	_float m_fWidth{};
	deque<CENTER_POINT> m_CenterPoints;

	/* Segment Mode */
	deque<SEGMENT_POINT> m_SegmentPoints;

public:
	static CTrailModel* Create();
	CComponent* Clone() override;
	virtual void Free() override;

};
NS_END
