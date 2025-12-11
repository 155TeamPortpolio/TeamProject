#pragma once
#include "Engine_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)


using Resource = unordered_map<string, any>; //-> 이건 특정 자료형을 담는 언오더드 맵임. (리소스 키와, 리소스 자체)

class ResourcePool {
public:
	template<typename T>
	HRESULT Add_BaseResource(T resource, const string& imguiID) {

		auto iter = m_Resources[type_index(typeid(T))].emplace(imguiID, resource);

		if (iter->second == true)
			return S_OK;
		else
			return E_FAIL;
	}

	template<typename T>
	T Find_Resource(const string& imguiID) {
		auto iter = m_Resources.find(type_index(typeid(T)));

		if (iter == m_Resources.end())
		{
			MSG_BOX("Wrong Resource Type Requested : ResourcePool");
			return T() ;
		}
		auto resourceIter = iter->second.find(imguiID);

		if (resourceIter == iter->second.end()) {
			MSG_BOX("Wrong Resource Key Requested : ResourcePool");
			return T();
		}
		return any_cast<T>(resourceIter->second);
	}

	void ClearAll() {
		for (auto& pair : m_Resources) {

			pair.second.clear();
		}
		m_Resources.clear();
	}
private:
	unordered_map<type_index, Resource> m_Resources; //이건 각 자료형을 기준으로 나누는 리소스
};
NS_END