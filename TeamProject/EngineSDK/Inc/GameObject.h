#pragma once
#include "Base.h"
#include "Transform.h"
#include "Builder.h"
#include "Collider.h"
#include "Model.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject abstract :
public CBase
{
protected:
   CGameObject();
   CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() DEFAULT;

public:
	template<typename T, typename ...Args>
	T* Add_Component(Args&& ...args);
	template<typename T>
	T* Get_Component();
	template<typename T>
	HRESULT Remove_Component();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(INIT_DESC* pArg = nullptr);
	virtual void Awake() {};
	virtual void Pre_EngineUpdate(_float dt);
	virtual void Post_EngineUpdate(_float dt);
	virtual void Priority_Update(_float dt) PURE;
	virtual void Update(_float dt) PURE;
	virtual void Late_Update(_float dt) PURE;

public:
	virtual void OnCollisionEnter() {};
	virtual void OnCollisionStay() {};
	virtual void OnCollisionExit() {};
	virtual void OnTriggerEnter() {};
	virtual void OnTriggerExit() {};

	virtual void OnCollisionEnter(CGameObject* pOther) {};
	virtual void OnCollisionStay(CGameObject* pOther) {};
	virtual void OnCollisionExit(CGameObject* pOther) {};
	virtual void OnTriggerEnter(CGameObject* pOther) {};
	virtual void OnTriggerStay(CGameObject* pOther) {};
	virtual void OnTriggerExit(CGameObject* pOther) {};

public:
	_bool Has_Tag(const string& tag) { return m_InstanceTag == tag; };
	void Set_Tag(const string& tag) { m_InstanceTag = tag; };
	string Get_Tag() { return m_InstanceTag ; };

public:
	virtual void Render_GUI();
	void RenderHierarchy(CGameObject*& outSelected, bool isSelected);

public:
	void Set_Layer(class CLayer* pLayer);
	class CLayer* Get_Layer() { return m_pLayer; };
	void Set_Level(const string& tag) { m_LevelTag = tag; };
	string Get_Level() { return m_LevelTag; };

public:
	LAYER_DESC Get_LayerDesc();
	OBJECT_HANDLE Get_Handle();
public:
	const string& Get_InstanceName() { return m_InstanceName; }
	const _uint Get_ObjectID() { return m_ObjectID; }
	/*----------------------------------------*/
	_float4x4* Get_WorldMatrix_Ptr();
	_float4 Get_Position();

	Matrix Get_WorldMatrix();
	_vector3 Get_WorldPos();
	_quaternion Get_WorldQuat();
	_vector3 Get_WorldRotation();
	/*----------------------------------------*/
	_bool Is_Root();
	const vector<CGameObject*> Get_Children();

public:
	_bool Is_Alive() { return m_isAlive; };
	void Set_Alive(_bool alive) { m_isAlive = alive; };

public:
	void Set_FromPool(_bool fromPool) { m_PoolMark.fromPool = fromPool; }
	bool IsFromPool() const { return m_PoolMark.fromPool; }
	void Set_PoolKey(CLONE_DESC key) { m_PoolMark.key = key; }
	const CLONE_DESC& Get_PoolKey() const { return m_PoolMark.key; }

	virtual void OnPooledAcquire(INIT_DESC* pArg = nullptr) {}		// 풀에서 꺼낼 때
	virtual void OnPooledRelease() {}														// 풀로 돌아갈 때

public:
	void SetRenderLayer(RENDER_LAYER layer) { m_eRenderLayer = layer; };
	RENDER_LAYER GetRenderLayer() const {return m_eRenderLayer; };

private:
	HRESULT Make_OpaquePacket();
	HRESULT Make_BlendedPacket(OPAQUE_PACKET packet);
	HRESULT Make_NonLightPacket(OPAQUE_PACKET packet);
	HRESULT Make_InstancePacket();
	HRESULT Make_ParticlePacket();
	HRESULT Make_EffectPacket(OPAQUE_PACKET packet);
	HRESULT Make_3DUIPacket(OPAQUE_PACKET packet);

private:
	_float Calculate_LinearDepth(const MINMAX_BOX& box);
protected:
	_bool m_isRootObject = { true };
	_uint m_ObjectID = {};

	CTransform* m_pTransform = { nullptr };
	class CLayer* m_pLayer = { nullptr };
	string m_LevelTag = {};
	
	string m_InstanceName = {};
	string m_InstanceTag = {};
	_bool m_isAlive = { true };

	map<type_index,class CComponent*> m_Components;

protected:
	RENDER_LAYER m_eRenderLayer = { RENDER_LAYER::Default };
	POOL_MARK m_PoolMark;

public:
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr)PURE;
	virtual void Free();
public:
	static _uint s_NextID;
};

NS_END

template<typename T, typename ...Args>
inline T* CGameObject::Add_Component(Args && ...args)
{
	T* comp = Get_Component<T>();
	if (comp)
		return comp;

	comp = T::Create(forward<Args>(args)...);

	if (comp) {
		m_Components.insert({ type_index(typeid(T)), comp });
		comp->Set_Owner(this);
	}

	if constexpr (is_base_of_v<CModel, T>) //모델 특수 처리
	{
		m_Components.insert({ type_index(typeid(CModel)), comp });
		Safe_AddRef(comp);
	}

	if constexpr (is_base_of_v<ICollidable, T>)
	{
		m_Components.insert({ type_index(typeid(ICollidable)), comp });
		Safe_AddRef(comp);
	}

	return comp;
}

template<typename T>
inline T* CGameObject::Get_Component()
{
	auto iter = m_Components.find(type_index(typeid(T)));

	if (iter != m_Components.end())
		return static_cast<T*>(iter->second);

	return nullptr;
}

template<typename T>
inline HRESULT CGameObject::Remove_Component()
{
	auto iter = m_Components.find(type_index(typeid(T)));

	if (iter != m_Components.end()) {
		if constexpr (is_base_of_v<CModel, T>) {
			Safe_Release(iter->second);
			m_Components.erase(type_index(typeid(CModel)));
		}
		if constexpr (is_base_of_v<ICollidable, T>)
		{
			Safe_Release(iter->second);
			m_Components.erase(type_index(typeid(ICollidable)));
		}
		Safe_Release(iter->second);
		m_Components.erase(iter);
		return S_OK;
	}
	else {
		return E_FAIL;
	}
}
