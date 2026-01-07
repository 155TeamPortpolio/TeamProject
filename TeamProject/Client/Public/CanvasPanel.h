#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class IUI_Service;
NS_END

NS_BEGIN(Client)

class CCanvasPanel final : public CUI_Object
{
private:
	CCanvasPanel() {}
	CCanvasPanel(const CCanvasPanel& rhs) : CUI_Object(rhs) {}
	virtual ~CCanvasPanel() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void    Priority_Update(_float dt)       override;
	virtual void    Update(_float dt)                override;
	virtual void    Late_Update(_float dt)		     override;
	virtual void    Render_GUI()				     override { __super::Render_GUI(); }

public:
	virtual void    Load(const nlohmann::ordered_json& data) override;

private:
	json LoadJson(const string& filePath);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free() override { __super::Free(); }
};

NS_END