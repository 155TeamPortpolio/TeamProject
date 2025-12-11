#include "Engine_Defines.h"
#include "CollisionSystem.h"
#include "GameInstance.h"
#include "ICameraService.h"
#include "Collider.h"

CCollisionSystem::CCollisionSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_pDevice{ pDevice }, m_pContext{ pContext }
{
	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);
}

HRESULT CCollisionSystem::Initialize()
{

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
	size_t      iShaderByteCodeLength = {};

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;
#endif
	m_Colliders.reserve(1000);

	return S_OK;
}

void CCollisionSystem::Update(_float dt)
{
	Clean_Up();
	for (auto& slot : m_Colliders)
	{
		if (slot.eState == COLLIDER_SLOT::STATE::DEAD ||
			slot.pCollider == nullptr)
			continue;
		
		if (!slot.pCollider->Get_CompActive())
			continue;

		slot.pCollider->Update();
	}

	MakeCandidate();

	for (size_t i = 0; i < m_CandidateCollision.size(); i++)
	{
		_int firstIndex = m_CandidateCollision[i].first;
		_int SecondIndex = m_CandidateCollision[i].second;

		m_Colliders[firstIndex].pCollider->Intersect(&m_Colliders[SecondIndex]);
		m_Colliders[SecondIndex].pCollider->Intersect(&m_Colliders[firstIndex]);
	}
}

void CCollisionSystem::Late_Update(_float dt)
{
	Clean_Up();
	for (auto& col : m_Colliders) {

		if (col.eState == COLLIDER_SLOT::STATE::DEAD ||
			col.pCollider == nullptr)
			continue;

		if (!col.pCollider->Get_CompActive())
			continue;

		col.pCollider->Late_Update();
	}
}

_int CCollisionSystem::RegisterCollider(CCollider* pCollider, _int Index)
{
	if (Index != -1 && Index < m_Colliders.size()) {
		if (m_Colliders[Index].pCollider == pCollider) {
			return Index;
		}
	}

	for (size_t i = 0; i < m_Colliders.size(); ++i)
	{
		if (m_Colliders[i].pCollider == nullptr && m_Colliders[i].eState == COLLIDER_SLOT::STATE::DEAD)
		{
			m_Colliders[i].pCollider = pCollider;
			m_Colliders[i].eState = (pCollider->Has_Desc() ? COLLIDER_SLOT::STATE::ACTIVE : COLLIDER_SLOT::STATE::INACTIVE);
			m_Colliders[i].iGeneration++;
			return static_cast<_int>(i);
		}
	}

	COLLIDER_SLOT info{};
	info.pCollider = pCollider;
	info.eState = (pCollider->Has_Desc() ? COLLIDER_SLOT::STATE::ACTIVE : COLLIDER_SLOT::STATE::INACTIVE);
	info.iGeneration = 1;
	m_Colliders.push_back(info);
	return static_cast<_int>(m_Colliders.size() - 1);
}


void CCollisionSystem::UnregisterCollider(CCollider* pCollider, _int Index)
{
	if (Index >= m_Colliders.size()) {
		return;
	}

	if (m_Colliders[Index].pCollider != pCollider) {
		return;
	}

	else {/*Remove At Clean Up*/
		m_Colliders[Index].eState = COLLIDER_SLOT::STATE::DEAD;
	}
}

void CCollisionSystem::DeActiveCollider(CCollider* pCollider, _int Index)
{
	if (Index >= m_Colliders.size()) {
		return;
	}

	if (m_Colliders[Index].pCollider != pCollider) {
		return;
	}

	else {
		m_Colliders[Index].eState = COLLIDER_SLOT::STATE::INACTIVE;
	}
}

void CCollisionSystem::ActiveCollider(CCollider* pCollider, _int Index)
{
	if (Index >= m_Colliders.size()) {
		return;
	}
	if (m_Colliders[Index].pCollider != pCollider) {
		return;
	}
	if (!pCollider->Has_Desc()) {
		return;
	}

	else {
		m_Colliders[Index].eState = COLLIDER_SLOT::STATE::ACTIVE;
	}
}

void CCollisionSystem::MakeCandidate()
{
	m_CandidateCollision.clear();
	m_CandidateCollision.reserve(m_Colliders.size());

	for (size_t i = 0; i < m_Colliders.size(); i++)
	{
		if (m_Colliders[i].IsValid() == false)
			continue;
		if (m_Colliders[i].IsActive() == false)
			continue;
		if (m_Colliders[i].pCollider->Get_CompActive() == false)
			continue;

		for (size_t j = i + 1; j < m_Colliders.size(); j++)
		{
			if (m_Colliders[j].IsValid() == false)
				continue;
			if (m_Colliders[j].IsActive() == false)
				continue;
			if (m_Colliders[j].pCollider->Get_CompActive() == false)
				continue;

			m_CandidateCollision.emplace_back(i, j);
		}
	}
}

void CCollisionSystem::Clean_Up()
{
	for (auto& col : m_Colliders) {
		if (!col.pCollider) continue;

		if (false == col.pCollider->Get_CompActive()) {
			col.pCollider = nullptr;
			col.eState = COLLIDER_SLOT::STATE::DEAD;
		}
	}
}

#ifdef _DEBUG
void CCollisionSystem::Render_Debug()
{
	auto camMgr = CGameInstance::GetInstance()->Get_CameraMgr();
	_matrix viewMat = XMLoadFloat4x4(camMgr->Get_ViewMatrix());
	_matrix projMat = XMLoadFloat4x4(camMgr->Get_ProjMatrix());

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(viewMat);
	m_pEffect->SetProjection(projMat);

	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();
	m_pContext->GSSetShader(nullptr, nullptr, 0); // ¡ç ¿©±â!
	for (size_t i = 0; i < m_Colliders.size(); i++)
	{
		if (m_Colliders[i].IsActive()) {
			m_Colliders[i].pCollider->Render(m_pBatch, XMVectorSet(0.f, 1.f, 0.f, 1.f));
		}
	}

	m_pBatch->End();
}
#endif 

CCollisionSystem* CCollisionSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCollisionSystem* Instance = new CCollisionSystem(pDevice, pContext);
	if (FAILED(Instance->Initialize())) {
		Safe_Release(Instance);
	}
	return Instance;
}

void CCollisionSystem::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_Colliders.clear();

#ifdef _DEBUG
	Safe_Release(m_pInputLayout);
	Safe_Delete(m_pEffect);
	Safe_Delete(m_pBatch);
#endif // _DEBUG
}