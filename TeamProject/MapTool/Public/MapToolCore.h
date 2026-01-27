#pragma once
#include "Base.h"
#include "MapTool_Context.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(MapTool)
class CBattleObject;
class CMapToolCore final : public CBase
{
	DECLARE_SINGLETON(CMapToolCore)
private:
	CMapToolCore();
	virtual ~CMapToolCore() = default;

public:
	class CMapToolGui*	Get_MapToolGui() { return m_pMapToolGui; }
	MAPTOOL_CONTEXT*	Get_Context() { return &m_tMapToolContext; }
	void				Set_AllObjectDebugRender(_bool is);

public:
	LOADED_DATA			Load_MapData();
	void				Load_WithEntityData();
	void				Clear_Layer(MAPOBJ_TYPE eObjType);
	void				RegisterGuiPanel(class CMapToolGui* pGUIPanel);
	

private:
	MAPOBJ_TYPE		Check_LayerTag(const string& TagLayer);
	void			Place_PlacedObjectFromLoadData(MapData_Object* pData);
	void			Place_TriggerObjectFromLoadData(MapData_Object* pData);
	void			Place_EntityObjectFromLoadData(ENTITY* pData);


private:
	CGameInstance*		m_pGameInstance = { nullptr };

	MAPTOOL_CONTEXT		m_tMapToolContext = {};
	class CMapToolGui*	m_pMapToolGui = { nullptr };
public:
	static CMapToolCore* Create();
	virtual void Free() override;
};

NS_END