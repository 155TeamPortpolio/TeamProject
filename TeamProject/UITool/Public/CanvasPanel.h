#pragma once
#include "UIObject_Tool.h"

NS_BEGIN(Engine)
class IUI_Service;
NS_END

NS_BEGIN(UITool)

class CCanvasPanel final : public CUIObject_Tool
{
private:
	CCanvasPanel() {}
	CCanvasPanel(const CCanvasPanel& rhs) : CUIObject_Tool(rhs) {}
	virtual ~CCanvasPanel() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override;
	virtual void    Update(_float dt)                override;
	virtual void    Late_Update(_float dt)           override;						    
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	
public:
	virtual void	Save(nlohmann::ordered_json& data) override;
	
private:
	const _float2	m_fChildCreateSize = { 100.f, 100.f };

public:
	static const string m_strTypeTag;
	static _uint        m_iCount;

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free() { __super::Free(); }
};

NS_END