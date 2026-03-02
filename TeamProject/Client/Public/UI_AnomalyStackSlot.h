#pragma once
#include "UI_Object.h"

NS_BEGIN(Client)

class CUI_AnomalyStackSlot final : public CUI_Object
{
private:
	enum class STATE { FULL, EMPTY, COUNT };
	enum class FIRE { START, LOOP, END, COUNT };

	enum class CHILD { FILL, OVERLAY, START, LOOP, END, COUNT };
	inline static const string INSTANCENAMES[ENUM(CHILD::COUNT)] = { "fill", "overlay", "start", "loop", "end" };

private:
	CUI_AnomalyStackSlot() {}
	CUI_AnomalyStackSlot(const CUI_AnomalyStackSlot& rhs) : CUI_Object(rhs) {}
	virtual ~CUI_AnomalyStackSlot() DEFAULT;

public:
	void Activate(_bool isActive);
	void PlayEffect();
	void StopEffect();

public:
	virtual HRESULT Initialize_Prototype()           override;
	virtual HRESULT Initialize(INIT_DESC* pArg = {}) override;
	virtual void	Awake()							 override;
	virtual void    Priority_Update(_float dt)       override { __super::Priority_Update(dt); }
	virtual void    Update(_float dt)			     override;
	virtual void    Late_Update(_float dt)           override { __super::Late_Update(dt); }
	virtual void    Render_GUI()                     override { __super::Render_GUI(); }
	virtual void	UI_Active(void* pArg = nullptr)	 override;
	virtual void	UI_DeActive(void* pArg = nullptr) override;

private:
	FIRE m_eFire = { FIRE::COUNT };
	STATE m_eState = { STATE::COUNT };

	CUI_Object* m_pChildren[ENUM(CHILD::COUNT)] = {};

private:
	void Cache(); 

	void Change_State(STATE eState);
	void Change_Fire(FIRE eFire);

	void SetFill(_bool isFill);
	void PlayFireStart();
	void PlayFireLoop();
	void PlayFireEnd();

	void Set_ChildAlive(CHILD child, _bool isAlive);
	void Set_ChildAnimation(CHILD child, _int iIndex);
	void Set_ChildSpriteAnimation(CHILD child, _bool isActive);

	_bool Is_AnimationFinished(CHILD child);

public:
	static  CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = {}) override;
	virtual void Free() { __super::Free(); }
};

NS_END