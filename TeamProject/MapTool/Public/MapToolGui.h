#pragma once
#include "BasePanel.h"
#include "MapTool_Context.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(MapTool)

class CMapToolGui final : public CBasePanel
{
public:
	struct TriggerTransform {
		COLLIDER_TYPE	eType = { COLLIDER_TYPE::BOX };

		_float3 vScale = { 1.f, 1.f, 1.f };
		_float3 vTranslation = { 0.f, 0.f, 0.f };
		_float3	vRotation = { 0.f, 0.f, 0.f };
	};

private:
	CMapToolGui(GUI_CONTEXT* pContext);
	virtual ~CMapToolGui() = default;

public:
	HRESULT			Initialize();
	virtual void	Update_Panel(_float dt) override;
	virtual void	Render_GUI() override;

private:
	void			RakeResources();
	void			CheckCoolTime(_float dt);
	void			KeyInput();
	void			Compute_Ray();
	void			Place_Object(PHYSICS_RAY_HIT* pRayHit);
	// 기존 레이피킹 방식. 사용X
	void			Set_ObjectPicking(_bool is);
	void			PreSet_ModelResource();
	void			Save_MapData();
	void			Select_PlaceType();
	void			Select_TriggerType();

	void			Render_ClearLayer();

private:
	/* Refernce */
	CGameInstance*			m_pGameInstance = { nullptr };
	class CMapToolCore*		m_pMapToolCore = { nullptr };
	class CSlotFieldGui*	m_pSlotFieldGui = { nullptr };
	class CMapToolAssistant* m_pAssistant = { nullptr };
	MAPTOOL_CONTEXT*		m_pMapToolContext = { nullptr };


	// Physics Ray
	PHYSICS_RAY	m_PhysicsRay = {};
	_float3		m_vRayHitPos = {};

	/* For.RakeResouece */
	vector<ModelPathPack>		m_ModelPathPack;

	/* For.Object */
	_int		m_iSelectedModelIndex = { -1 };
	string		m_TagSelectedModelName = {};
	_bool		m_isObjectPicking = { true };
	_float3		m_vScale_PlacedObject = { 1.f, 1.f, 1.f };
	_int		m_iSelectedLayerIndex = {};
	
	/* For.Trigger */
	TriggerTransform	m_TriggerTransform = {};
	_int		m_iTriggerIndex = {};

	/* For.Data */
	MapData_Header	m_Data = {};
	//string			m_TagArea = {};

	_float2			m_vShowSaveFinish = {};
	_bool			m_isShowSaveFinish = { false };

public:
	static CMapToolGui* Create(GUI_CONTEXT* pContext);
	virtual void Free() override;
};

NS_END