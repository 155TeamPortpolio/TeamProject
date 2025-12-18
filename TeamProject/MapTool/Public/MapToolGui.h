#pragma once
#include "BasePanel.h"

NS_BEGIN(Engine)
class CGameInstance;

NS_END

NS_BEGIN(MapTool)

class CMapToolGui final : public CBasePanel
{
private:
	CMapToolGui(GUI_CONTEXT* pContext);
	virtual ~CMapToolGui() = default;

public:
	HRESULT			Initialize();
	virtual void	Update_Panel(_float dt) override;
	virtual void	Render_GUI() override;

private:
	void			RakeResources();
	void			KeyInput();
	void			Compute_Ray();
	void			Place_Object(PHYSICS_RAY_HIT* pRayHit);
	// 기존 레이피킹 방식. 사용X
	void			Set_ObjectPicking(_bool is);
	void			PreSet_ModelResource();
	void			Save_MapData();
	void			Load_MapData();
	void			Place_PlacedObjectFromLoadData(MapData_Object* pData);
	void			Clear_Layer();
	


private:
	CGameInstance* m_pGameInstance = { nullptr };

	// BG
	//RAY			m_Ray = {};
	_float3		m_vRayHitPos = {};

	// Physics Ray
	PHYSICS_RAY	m_PhysicsRay = {};

	/* For.RakeResouece */
	vector<ModelPathPack>		m_ModelPathPack;

	/* For.Object */
	string		m_TagPlacedObjectLayer = {};
	string		m_TagFloorObjectLayer = {};
	string		m_TagTriggerObjectLayer = {};

	_int		m_iSelectedIndex = { -1 };
	string		m_TagSelectedModelName = {};
	_bool		m_isObjectPicking = { true };
	_float3		m_vScale_PlacedObject = { 1.f, 1.f, 1.f };
	
	/* For.Data */
	vector<string>	m_TagLayers;
	_int			m_iVersion = { 1 };
	MapData_Header	m_Data = {};

public:
	static CMapToolGui* Create(GUI_CONTEXT* pContext);
	virtual void Free() override;
};

NS_END