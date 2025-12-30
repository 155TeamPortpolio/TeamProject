#pragma once
#include "Component.h"
#include "EventSystem.h"

NS_BEGIN(Engine)
class ENGINE_DLL CEventListener :
    public CComponent
{
private:
	CEventListener();
	CEventListener(const CEventListener& rhs);
	virtual ~CEventListener() DEFAULT;
	CEventListener& operator=(const CEventListener&) = delete;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(COMPONENT_DESC* pArg) override;

public:
	template <typename T>
	void Add_Listner(function<void(const T&)> callback);
	void Clear(){m_Handles.clear(); }

public:
	void Set_CompActive(_bool bActive) override;

public:
	void Render_GUI();

private:
	CEventSystem* m_pSystem = { nullptr };
	vector<CEventSystem::ListenerHandle> m_Handles;
public:
	static CEventListener* Create();
	virtual CComponent* Clone() override;
	virtual void Free() override;
};

template<typename T>
inline void CEventListener::Add_Listner(function<void(const T&)> callback)
{
	if (!m_pSystem) return;
	CGameObject* owner = Get_Owner(); 
	m_Handles.emplace_back(m_pSystem->Subscribe<T>(owner, move(callback)));
}
NS_END
