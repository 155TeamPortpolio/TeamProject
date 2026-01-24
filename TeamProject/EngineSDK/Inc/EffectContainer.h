#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class ENGINE_DLL CEffectContainer :
    public CGameObject
{
public:
	typedef struct tagEffectContainerContext
	{
		_float3 vLinePoint0{};
		_float3 vLinePoint1{};

	}EFFECT_CONTAINER_CONTEXT;
protected:
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
	EFFECT_CONTAINER_CONTEXT& GetEffectContext();
	void SetLinePoints(_float3 point0, _float3 point1);
	void AttachBone(class CAnimator3D* pAnimator, const string& boneTag, _fmatrix offsetMatrix = _smatrix::Identity);

public:
	void Play();
	void Stop();
	_bool IsLoop()const { return m_IsLoop; }

public:
	static CEffectContainer* Create();
	CGameObject* Clone(INIT_DESC* pArg) override;
	virtual void Free();

protected:
	_bool m_IsBillBoard = false;
	_bool m_IsLoop = false;
	_float m_fDuration{};
	_float m_fElapsedTime{};
	_uint m_iNumNodes{};
	EFFECT_CONTAINER_CONTEXT m_EffectContext{};
	vector<class CEffectNode*> m_Nodes;

};
NS_END