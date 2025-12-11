#pragma once
#include "UI_Object.h"

NS_BEGIN(UITool)

class CTestUI final : public CUI_Object
{
private:
	CTestUI();
	CTestUI(const CTestUI& rhs);
	virtual ~CTestUI() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(INIT_DESC* pArg = nullptr) override;

	virtual void Priority_Update(_float dt) override;
	virtual void Update(_float dt) override;
	virtual void Late_Update(_float dt) override;

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free();
};

NS_END