#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class CEffectContainer :
    public CGameObject
{
private:
	CEffectContainer();
	CEffectContainer(const CEffectContainer& rhs);
	virtual ~CEffectContainer() DEFAULT;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(INIT_DESC* pArg) override;
	void Awake() override;
	void Priority_Update(_float dt) override;
	void Update(_float dt) override;
	void Late_Update(_float dt) override;

public:
	static CEffectContainer* Create();
	CGameObject* Clone(INIT_DESC* pArg) override;
	virtual void Free();

private:
	_bool m_IsLoop = false;
	_float m_fDuration{};
	_float m_fElapsedTime{};
	_uint m_iNumNodes{};
	vector<class CEffectNode*> m_Nodes;
};
NS_END