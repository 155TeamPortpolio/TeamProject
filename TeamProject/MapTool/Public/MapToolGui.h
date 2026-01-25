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

public:
	void			RakeResources();
	void			CheckCoolTime(_float dt);
	void			KeyInput();
	void			Compute_Ray();
	void			Place_Object(PHYSICS_RAY_HIT* pRayHit);
	void			Place_BattleData(PHYSICS_RAY_HIT* pRayHit);
	// 기존 레이피킹 방식. 사용X
	void			Set_ObjectPicking(_bool is);
	void			PreSet_ModelResource();
	void			Save_MapData();
	void			Save_EntityData();
	void			Save_MapToolEntityData();
	void			Save_EntityInit();
	void			Load_EntityInit();
	void			Set_EntityModel();
	void			Save_BattleData();
	void			Load_BattleData(const string& filepath = "");
	void			Select_PlaceType(const string& tagLabel, _bool isShowDetail = true);
	void			Select_TriggerType();
	void			Select_BattleDataType();
	void			Setting_SelectType();

	void			Render_ClearLayer();

private:
	/* Refernce */
	CGameInstance* m_pGameInstance = { nullptr };
	class CMapToolCore* m_pMapToolCore = { nullptr };
	class CSlotFieldGui* m_pSlotFieldGui = { nullptr };
	class CMapToolAssistant* m_pAssistant = { nullptr };
	MAPTOOL_CONTEXT* m_pMapToolContext = { nullptr };

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

	/* For.Entity */
	_float3				 m_vEntitySize = { 1.f, 1.f, 1.f };
	_int				 m_iEntityIndex = {};

	_int				 m_iPickedEntityModelIndex{ -1 };
	vector<string>		 m_EntityModelPathPackName{};
	class CEntityObject* m_pSelectedEntityObject;
	unordered_map<string, string> m_iniModelName;

	/* For.BattleData */
	BATTLE_TYPE			m_eBattlyDataType = {};
	_float3				m_vBattleDataSize = { 1.f, 1.f, 1.f };;
	_int				m_iSpawnerIndex = {};
	_int				m_iMonsterIndex = {};
	_int				m_iEndPointIndex = {};

	/* For.Data */
	MapData_Header	m_MapData = {};
	Entity_Header	m_EntityData = {};
	BATTLE_FIELD_DATA m_BattleData = {};
	///_uint			m_iBattleTableIndex = {};
	_float2			m_vShowDataSaveFinish = {};
	_bool			m_isShowDataSaveFinish = { false };

public:
	static CMapToolGui* Create(GUI_CONTEXT* pContext);
	virtual void Free() override;
};

NS_END