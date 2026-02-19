#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class CTextSlot;
NS_END

NS_BEGIN(Client)

class CUI_Tutorial final : public CUI_Object
{
public:
	typedef struct tagUITutorialDesc{
		TUTORIAL_TYPE eType = {};
	}UI_TUTORIAL_DESC;

private:
	CUI_Tutorial() {}
	CUI_Tutorial(const CUI_Tutorial& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_Tutorial() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg = nullptr) override;
	virtual void	UI_DeActive(void* pArg = nullptr) override;

private:
	TUTORIAL_TYPE m_eType = {};

	_bool m_isCheck = {};

	class CTextSlot* m_pTitle = {};
	
	map<TUTORIAL_TYPE, CUI_Object*> m_Descriptions;

	class CUI_TutorialVideo* m_pVideo = {};

private:
	void Cache();
	HRESULT Create_ExitButton();
	HRESULT Create_EnterButton();
	HRESULT Create_TutorialDescriptions();
	HRESULT Create_TutorialVideo();

	void Change_Description(TUTORIAL_TYPE eType);
	void Change_TitleText(TUTORIAL_TYPE eType);

	wstring Get_TitleText(TUTORIAL_TYPE eType);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() override;
};

NS_END