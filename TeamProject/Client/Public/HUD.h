#pragma once
#include "UIPrefab.h"

NS_BEGIN(Client)

class CHUD final : public CUIPrefab
{
private:
	CHUD();
	CHUD(const CHUD& rhs);
	virtual ~CHUD() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(INIT_DESC* pArg = nullptr) override;
	virtual void Priority_Update(_float dt) override;
	virtual void Update(_float dt) override;
	virtual void Late_Update(_float dt) override;

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free() override;
};

NS_END