#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUIPrefab abstract : public CUI_Object
{
protected:
	CUIPrefab();
	CUIPrefab(const CUIPrefab& rhs);
	virtual ~CUIPrefab() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(INIT_DESC* pArg = nullptr) override;
	virtual void Priority_Update(_float dt) override;
	virtual void Update(_float dt) override;
	virtual void Late_Update(_float dt) override;

protected:
	CUI_Object* pCanvasPanel = { nullptr };

public:
	virtual void Free() override;
};

NS_END