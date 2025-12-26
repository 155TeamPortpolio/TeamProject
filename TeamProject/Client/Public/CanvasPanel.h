#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class IUI_Service;
NS_END

NS_BEGIN(Client)

class CCanvasPanel final : public CUI_Object
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
	virtual void ReadElementData(const UI_ELEMENT_DATA& data) override;

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free();
};

NS_END