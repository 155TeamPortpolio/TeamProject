#pragma once
#include "UIObject_Tool.h"

// 부모가 스케일, 회전하면 자식도 영향 받게 수정해야

NS_BEGIN(Engine)
class IUI_Service;
NS_END

NS_BEGIN(UITool)

class CCanvasPanel final : public CUIObject_Tool
{
private:
	CCanvasPanel();
	CCanvasPanel(const CCanvasPanel& rhs);
	virtual ~CCanvasPanel() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(INIT_DESC* pArg = nullptr) override;

	virtual void Priority_Update(_float dt) override;
	virtual void Update(_float dt) override;
	virtual void Late_Update(_float dt) override;

	virtual void Render_GUI() override;
	
public:
	virtual void SavePrefab(json& data) override;
	virtual void LoadPrefab(const json& data) override;
	
private:
	const _float2		m_fChildCreateSize = { 100.f, 30.f };

public:
	static const string m_strTypeTag;
	static _uint m_iCount;

private:
	void Render_GUI_Create();
	void Render_GUI_SavePrefab();
	void Render_GUI_LoadPrefab();

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free();
};

NS_END