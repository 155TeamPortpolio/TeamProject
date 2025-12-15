#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameObject;

class CEventSystem :
	public CBase
{
public:
	/*듣는 이에 대한 핸들 -> 핸들을 통해서 청취자를 관리. 핸들 구조체가 사라질 때 자동으로 구독 취소*/ 
	struct ListenerHandle
	{
		CEventSystem* system = { nullptr };
		type_index type = { typeid(void) };
		_uint hID = { 0 };

		void Reset() 
		{
			if (system && hID != 0)
				system->Unsubscribe(type, hID); //여기서 구독 취소
			system = nullptr;
			hID = 0;
			type = typeid(void);
		}
		~ListenerHandle() { Reset(); }
		ListenerHandle() DEFAULT;
		ListenerHandle(const ListenerHandle&) = delete;
		ListenerHandle& operator=(const ListenerHandle&) = delete;
		ListenerHandle(ListenerHandle&& rhs) noexcept { *this = move(rhs); }
		ListenerHandle& operator=(ListenerHandle&& rhs) noexcept
		{
			if (this != &rhs)
			{
				Reset();
				system = rhs.system; type = rhs.type; hID = rhs.hID;
				rhs.system = nullptr; rhs.hID = 0; rhs.type = typeid(void);
			}
			return *this;
		}
	};

	/*듣는 이들을 관리하는 구조체 -> 어떤 타입이든 받을 수 있게 구성*/
	struct TypeSafeStruct {
		virtual ~TypeSafeStruct() = default;
		virtual void RemoveByOwner(class CGameObject* pOwner) PURE;
		virtual void RemoveById(uint64_t id) PURE;
	};
	template<typename T>
	struct TypeSafeVector : TypeSafeStruct
	{
		struct Entry /*누가 듣고, 들었을 떄 뭘 한다*/
		{
			CGameObject* owner = { nullptr };
			function<void(const T&)> callBack;
			_uint hID = 0;
			_bool alive = true;
		};
		vector<Entry> callbacks;
		_uint nextId = 1;
		_int dispatchDepth = 0; //중첩 호출 카운터 -> 현재 돌고 있는 친구가 있다면 삭제 방어하기 위함.

		void RemoveByOwner(CGameObject* pOwner) override/*오너 파괴될 때 자동으로 파괴될 수 있도록*/
		{
			for (auto& evt : callbacks)
				if (evt.owner == pOwner) 
					evt.alive = false;/*일단 생존 유무만 변화*/

			if (dispatchDepth == 0)
				Compact();/*현재 돌고 있는 친구 없으면 그제서야 삭제*/
		}
		inline void CEventSystem::TypeSafeStruct::RemoveById(uint64_t id)
		{
		}
		void RemoveById(_uint id) override
		{
			for (auto& evt : callbacks)
				if (evt.hID == id) { evt.alive = false; break; }

			if (dispatchDepth == 0)
				Compact();
		}
		void Compact()
		{
			callbacks.erase( /*청취자 없으면 해당 엔트리는 이제 삭제*/
				remove_if(callbacks.begin(), callbacks.end(),
					[](const Entry& evt) { return !evt.alive; }),
				callbacks.end());
		}
	};

private:
	CEventSystem();
	~CEventSystem() DEFAULT;

public:
	HRESULT Initialize();

public:
	template<typename Object, typename T>
	ListenerHandle Subscribe(Object* owner, void(Object::* method)(const T&)) /*오너와 함수 포인터 제공*/
	{
		/*엔트리를 등록*/
		static_assert(std::is_base_of_v<CGameObject, Object>,
			"Subscribe owner must derive from CGameObject");

		/*엔트리 있으면 가져오고, 없으면 생성*/
		auto* vec = GetEntryContainer<T>();

		/*엔트리 구조체 하나 만듦*/
		typename TypeSafeVector<T>::Entry entry;
		entry.owner = owner;
		entry.hID = vec->nextId++; /*이건 다음 아이디 체크(벡터를 가진 TypeSafeVector가 관리 중)*/
		entry.callBack = [owner, method](const T& evt) { (owner->*method)(evt); }; /*콜백 등록*/
		/*벡터에 넣기*/
		vec->callbacks.push_back(move(entry));

		ListenerHandle hListner;
		hListner.system = this;
		hListner.type = type_index(typeid(T));
		hListner.hID = vec->callbacks.back().hID;
		return hListner;
	}
	void Unsubscribe(const type_index& type, _uint id)
	{
		auto it = m_Callbacks.find(type);
		if (it == m_Callbacks.end()) return;
		it->second->RemoveById(id); /*엔트리가 가상함수 이미 들고 있어서, 타입 아이디만 잘 주면 됨*/
	}

	template<typename T>
	void Broadcast(const T& data)
	{
		auto it = m_Callbacks.find(type_index(typeid(T)));
		if (it == m_Callbacks.end()) return; // 리스너 없으면 걍 그래도 넘어감

		/*찾으면 캐스팅*/
		auto* vec = static_cast<TypeSafeVector<T>*>(it->second);

		/*여기서 호출 카운터 증감 -> 지금 한명 청취 중입니다.*/
		vec->dispatchDepth++;
		for (auto& entry : vec->callbacks)
			if (entry.alive) entry.callBack(data);
		vec->dispatchDepth--;

		if (vec->dispatchDepth == 0)
			vec->Compact();
	}

private:
	template<typename T>
	TypeSafeVector<T>* GetEntryContainer()
	{
		const type_index idx = std::type_index(typeid(T));
		auto it = m_Callbacks.find(idx);
		if (it == m_Callbacks.end())
		{
			m_Callbacks[idx] = new TypeSafeVector<T>();
			it = m_Callbacks.find(idx);
		}
		return static_cast<TypeSafeVector<T>*>(it->second);
	}

private:
	unordered_map<type_index, TypeSafeStruct*> m_Callbacks;

public:
	static CEventSystem* Create();
	void Free() override;
};

NS_END
