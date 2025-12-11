#include "Engine_Defines.h"
#include "LightMgr.h"
#include "Light.h"

CLightMgr::CLightMgr()
{
}

CLightMgr::~CLightMgr()
{
}

HRESULT CLightMgr::Initialize()
{
	m_LightLifes.resize(10);
	m_Lights.resize(10);

	return S_OK;
}

_int CLightMgr::Register_Light(class CLight* Light)
{
	if (!m_LightLifes.empty()) {
		for (size_t i = 0; i < m_LightLifes.size(); i++)
		{
			if (m_LightLifes[i] == false) {
				m_LightLifes[i] = true;
				m_Lights[i]= Light;
				Safe_AddRef(Light);
				return i;
			}
			else
				continue;
		}
	}
	m_LightLifes.push_back(true);
	m_Lights.push_back(Light);
	Safe_AddRef(Light);
	return m_LightLifes.size() - 1;
}

void CLightMgr::UnRegister_Light(_int ID)
{
	if (m_LightLifes.size() <= ID || 0 > ID)
		return;

	m_LightLifes[ID] = false;
	Safe_Release(m_Lights[ID]);
}

CLightMgr* CLightMgr::Create()
{
	CLightMgr* instance = new CLightMgr();
	if (FAILED(instance->Initialize())) {
		Safe_Release(instance);
	}
	return instance;
}

void CLightMgr::Free()
{
	for (auto& light : m_Lights)
		Safe_Release(light);

	m_LightLifes.clear();
	m_Lights.clear();
}
