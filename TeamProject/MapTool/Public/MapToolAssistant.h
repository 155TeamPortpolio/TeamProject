#pragma once
#include "BasePanel.h"
#include "MapTool_Context.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(MapTool)

class CMapToolAssistant final : public CBasePanel
{
private:
	CMapToolAssistant(GUI_CONTEXT* pContext);
	virtual ~CMapToolAssistant() = default;

public:
	HRESULT			Initialize();
	virtual void	Update_Panel(_float dt) override;
	virtual void	Render_GUI() override;

public:
	void			Set_isOpen(_bool is) { m_isOpen = is; }
	_bool			IsOpen() { return m_isOpen; }

private:
	void			Rake_BoneData();
	void			LoadBoneData(filesystem::path& dir);
	void			SelectModel();
	void			SelectBoneData();
	void			Recommend_Transform();
	void			RecommendList();

private:
	/* Refernce */
	CGameInstance*		m_pGameInstance = { nullptr };
	class CMapToolCore*	m_pMapToolCore = { nullptr };
	MAPTOOL_CONTEXT*	m_pMapToolContext = { nullptr };

	_bool				m_isOpen = { false };
	vector<BONE_DATA_HEADER>	m_BoneData;
	vector<BONE_INFO>			m_RecommendList;
	_int				m_iSelectedModelIndex = {-1};

public:
	static CMapToolAssistant* Create(GUI_CONTEXT* pContext);
	virtual void Free() override;
};

NS_END