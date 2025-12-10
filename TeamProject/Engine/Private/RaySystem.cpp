#include "RaySystem.h"
#include "RayReceiver.h"

CRaySystem::CRaySystem()
{
}

CRaySystem::~CRaySystem()
{
}

HRESULT CRaySystem::Initialize()
{
	return S_OK;
}

_int CRaySystem::Register_RayReceiver(CRayReceiver* RayTarget)
{
	if (!m_RayReceivers.empty()) {
		for (size_t i = 0; i < m_ReceiversLifes.size(); i++)
		{
			if (m_ReceiversLifes[i] == false) {
				m_ReceiversLifes[i] = true;
				m_RayReceivers[i] = RayTarget;
				Safe_AddRef(RayTarget);
				return i;
			}
			else
				continue;
		}
	}
	m_ReceiversLifes.push_back(true);
	m_RayReceivers.push_back(RayTarget);
	Safe_AddRef(RayTarget);

	return m_ReceiversLifes.size() - 1;
}

void CRaySystem::UnRegister_RayReceiver(_int ID)
{
	if (m_ReceiversLifes.size() <= ID || 0 > ID)
		return;

	m_ReceiversLifes[ID] = false;
	Safe_Release(m_RayReceivers[ID]);
}

void CRaySystem::Register_Ray(RAY* ray)
{
	if (m_pRay == ray) return;

	m_pRay = ray;
}

void CRaySystem::Delete_Ray()
{
	m_pRay = { nullptr };
}

RAY_HIT* CRaySystem::Get_FrontRayHit()
{
	if (m_HittedTargets.empty())
		return nullptr;

	return &m_HittedTargets.front();
}

void CRaySystem::Update(_float dt)
{
	m_HittedTargets.clear();
	if (m_pRay == nullptr) return;

	for (auto rayTarget : m_RayReceivers)
	{
		RAY_HIT info = {};
		if (rayTarget->OnRayHit(m_pRay, &info)) {
			m_HittedTargets.push_back(info);
		};
	}

	sort(m_HittedTargets.begin(), m_HittedTargets.end(),
		[&](const RAY_HIT a, const RAY_HIT b) {
			return a.fDistance < b.fDistance;
		});
	
}

CGameObject* CRaySystem::Get_HittedObject()
{
	if (m_HittedTargets.empty())
		return nullptr;
	else {
		m_HittedTargets.front().pObject;
	}
}

CRaySystem* CRaySystem::Create()
{
	CRaySystem* instance = new CRaySystem();
	if (FAILED(instance->Initialize())) {
		Safe_Release(instance);
	}
	return instance;
}

void CRaySystem::Free()
{
	for (auto& rayReceiver : m_RayReceivers)
		Safe_Release(rayReceiver);

	m_ReceiversLifes.clear();
	m_RayReceivers.clear();
	m_HittedTargets.clear();
}
