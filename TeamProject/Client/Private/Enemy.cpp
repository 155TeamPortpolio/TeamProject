#include "pch.h"
#include "Enemy.h"
#include "BattleSystem.h"

CEnemy::CEnemy()
	:CGameObject()
{
}

CEnemy::CEnemy(const CEnemy& rhg)
	:CGameObject(rhg)
{
}

HRESULT CEnemy::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CEnemy::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CEnemy::Update(_float dt)
{
	m_PlayerCharacterInfos.clear();
	m_PlayerCharacterInfos = CBattleSystem::GetInstance()->GetBattleObjects(CBattleSystem::BATTLE_OBJ_TYPE::PLAYER);
	ComputeTargetingInfo();
}

BATTLEOBJ_INFO* CEnemy::GetCharacterOnField()
{
	// 추후에 캐릭터 여러명 나올 때 로직 나왔을 때 변경 예정
	for (auto& info : m_PlayerCharacterInfos) {
		if (true == info.isOnField)
			return &info;
	}
	return nullptr;
}

void CEnemy::ComputeTargetingInfo()
{
	auto pTargetInfo = GetCharacterOnField();
	if (nullptr == pTargetInfo)
		return;

	m_tTargetingInfo = {};
	
	m_tTargetingInfo.vTargetPos = pTargetInfo->vPos;
	m_tTargetingInfo.vSelfPos = m_pTransform->Get_Pos();
	m_tTargetingInfo.vDirSelfLook = m_pTransform->Dir(Engine::STATE::LOOK);
	m_tTargetingInfo.vDirSelfLook.Normalize();
	
	// Y축 제거 한 수평 방향 벡터 계산 버전(XZ평면)
	_vector3 vTargetPosH = { m_tTargetingInfo.vTargetPos.x, 0.f, m_tTargetingInfo.vTargetPos.z };
	_vector3 vSelfPosH = { m_tTargetingInfo.vSelfPos.x, 0.f, m_tTargetingInfo.vSelfPos.z };
	_vector3 vDirToTarget = vTargetPosH - vSelfPosH;

	// Y축 제거 안한 3D 방향 벡터 계산 버전(XYZ)
	//_vector3 vDirToTarget = m_tTargetingInfo.vTargetPos - m_tTargetingInfo.vSelfPos;

	m_tTargetingInfo.fDistanceSq = vDirToTarget.LengthSquared();
	if (m_tTargetingInfo.fDistanceSq <= m_fDetectedRange * m_fDetectedRange)
		m_tTargetingInfo.isDetected = true;

	// sqrt 계산이 비교적 무거워서 후에 최적화 필요시 아래 식 사용 고려
	//m_tTargetingInfo.fDistance = (m_tTargetingInfo.fDistanceSq > m_fDetectedRange * m_fDetectedRange) ? 
	//	sqrt(m_tTargetingInfo.fDistanceSq) : m_fDetectedRange;
	m_tTargetingInfo.fDistance = sqrt(m_tTargetingInfo.fDistanceSq); 

	// 혹시 모를 0 나누기 방지
	if (m_tTargetingInfo.fDistance > 1e-12f) {
		vDirToTarget.Normalize();
	}
	m_tTargetingInfo.vDirToTarget = vDirToTarget;

	// 평면 XZ상 내적 계산
	_vector3 vSelfLookH = m_tTargetingInfo.vDirSelfLook;
	vSelfLookH.y = 0.f;
	vSelfLookH.Normalize();
	m_tTargetingInfo.fDotTarget = vSelfLookH.Dot(m_tTargetingInfo.vDirToTarget);

	// 3D공간상 내적(XYZ)
	//m_tTargetingInfo.fDotTarget = m_tTargetingInfo.vDirSelfLook.Dot(m_tTargetingInfo.vDirToTarget);
}

void CEnemy::Render_GUI_ForTargetInfo()
{
	ImGui::PushID(this);

	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 8) + (ImGui::GetStyle().WindowPadding.y * 2);

	ImGui::SeparatorText("For Target Information");
	auto pCharacter = GetCharacterOnField();
	if (nullptr != pCharacter) {
		ImGui::BeginChild("TracePlayer##ThugBulkyEnforcerTracePlayer", ImVec2{ 0, childHeight }, true);

		ImGui::Text("Character Name : %s", pCharacter->TagInstanceName.c_str());
		ImGui::Text("Character Pos : %.2f, %.2f, %.2f", m_tTargetingInfo.vTargetPos.x, m_tTargetingInfo.vTargetPos.y, m_tTargetingInfo.vTargetPos.z);
		ImGui::Text("Character CCT Radius : %.2f", pCharacter->fRadius);;
		ImGui::Text("Distance From Character : %.3f", m_tTargetingInfo.fDistance);
		ImGui::Text("Dot with Target : %.2f", m_tTargetingInfo.fDotTarget);
		ImGui::Text("Dir To Target : %.2f, %.2f, %.2f", m_tTargetingInfo.vDirToTarget.x, m_tTargetingInfo.vDirToTarget.y, m_tTargetingInfo.vDirToTarget.z);
		ImGui::BeginDisabled(true);
		ImGui::Checkbox(u8"isDetected", &m_tTargetingInfo.isDetected);
		ImGui::EndDisabled();

		ImGui::EndChild();
	}
	ImGui::PopID();
}

void CEnemy::Free()
{
	__super::Free();
}
