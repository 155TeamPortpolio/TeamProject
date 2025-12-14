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
	m_LightSlots.resize(15);
	return S_OK;
}

/*라이트 매니저는 모든 라이트를 약한 참조로 관리하고 있음. 생성과 삭제는 모두 오브젝트가 컴포넌트를 담당해서 관리하고,
라이트 컴포넌트가 본인이 사라질 때, 자신이 사라짐을 알려줄 것임*/

_int CLightMgr::Register_Light(CLight* Light, _int Index)
{
	//유효한 인덱스라면,
	if (Index != -1 && Index < m_LightSlots.size()) {
		//그리고 같은 라이트라면
		if (m_LightSlots[Index].pLight == Light) {
			return Index;
		}
	}

	//등록 시에 순회 돌면서 살아있는 녀석으로 바꿈
	for (size_t i = 0; i < m_LightSlots.size(); ++i)
	{
		//죽은 녀석이라면
		if (m_LightSlots[i].pLight == nullptr && m_LightSlots[i].eState == LightState::DEAD)
		{
			m_LightSlots[i].pLight = Light;
			/*활성 컴포넌트라면 액티브 아니면 인액티브*/
			m_LightSlots[i].eState = (Light->Get_CompActive() ? LightState::ACTIVE : LightState::INACTIVE);
			m_LightSlots[i].descSnapShot = Light->SnapShot_Desc();
			return static_cast<_int>(i);
		}
	}

	//더이상 남은 빈공간이 없다면.

	LightSlot slot{};
	slot.pLight = Light;
	slot.eState = (Light->Get_CompActive() ? LightState::ACTIVE : LightState::INACTIVE);
	slot.generation = 1;
	slot.descSnapShot = Light->SnapShot_Desc();
	m_LightSlots.push_back(slot);
	return static_cast<_int>(m_LightSlots.size() - 1);
}

void CLightMgr::UnRegister_Light(CLight* Light, _int Index)
{
	if (m_LightSlots.size() <= Index || 0 > Index)
		return;

	if (m_LightSlots[Index].pLight != Light)
		return;

	else {
		m_LightSlots[Index].pLight = nullptr;
		m_LightSlots[Index].eState = LightState::DEAD;
	}
}

void CLightMgr::DeActive_Light(CLight* Light, _int Index)
{
	if (Index >= m_LightSlots.size()) {
		return;
	}

	if (m_LightSlots[Index].pLight != Light) {
		return;
	}

	else {
		m_LightSlots[Index].eState = LightState::INACTIVE;
	}
}

void CLightMgr::Active_Light(CLight* Light, _int Index)
{
	if (Index >= m_LightSlots.size()) {
		return;
	}

	if (m_LightSlots[Index].pLight != Light) {
		return;
	}
	if (!Light->Get_CompActive()) {
		return;
	}
	else {
		m_LightSlots[Index].eState = LightState::ACTIVE;
	}
}

vector<LIGHT_DESC> CLightMgr::Visible_Lights()
{
	CleanUp();

	vector<LIGHT_DESC> lights;

	for (auto& light : m_LightSlots)
	{
		if (light.eState == LightState::ACTIVE) {
			light.descSnapShot = light.pLight->SnapShot_Desc();
			lights.push_back(light.descSnapShot);
		}
	}

	return lights;
}

void CLightMgr::CleanUp()
{
	for (auto& light : m_LightSlots) {
		if (!light.pLight) {
			light.eState = LightState::DEAD;
			continue;
		};

		if (light.eState == LightState::DEAD) {
			light.pLight = nullptr;
			light.eState = LightState::DEAD;
			light.generation++; /*죽을 때만 세대 교체*/
		}
	}
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
	m_LightSlots.clear();
}
