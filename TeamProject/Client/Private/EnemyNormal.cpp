#include "pch.h"
#include "Enemy.h"
#include "GameInstance.h"
#include "BattleSystem.h"
#include "EnemyNormal.h"

CEnemyNormal::CEnemyNormal()
	: CEnemy()
{
}

CEnemyNormal::CEnemyNormal(const CEnemyNormal& rhg)
	: CEnemy(rhg)
{
}

HRESULT CEnemyNormal::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CEnemyNormal::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);


	return S_OK;
}

void CEnemyNormal::Update(_float dt)
{
	RotateToDir(dt);

	__super::Update(dt);
}

void CEnemyNormal::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

void CEnemyNormal::Free()
{
	__super::Free();
}

void CEnemyNormal::CaptureRotateToDir(_float3 vTargetDir, _float fSpeed)
{
	m_tRotDir.isLookPlayer = true;
	m_tRotDir.vDirToLookCapture = vTargetDir;
	m_tRotDir.fRotateSpeed = fSpeed;
	if (1 < m_tRotDir.fRotateSpeed)
		m_tRotDir.fRotateSpeed = 1;
}

void CEnemyNormal::RotateToDir(_float dt)
{
	if (false == m_tRotDir.isLookPlayer)
		return;

	//_vector vTargetDir = XMLoadFloat3(&m_tRotDir.vDirToLookCapture);
	//_vector	vSelfDir = m_pTransform->Dir(Engine::STATE::LOOK);
	//vTargetDir = XMVector3Normalize(vTargetDir);
	//vSelfDir = XMVector3Normalize(vSelfDir);
	//
	//_float fDot = XMVectorGetX(XMVector3Dot(vSelfDir, vTargetDir));
	//fDot = max(-1, min(1.f, fDot));
	//_float fAngle = acosf(fDot);
	//
	//_float fCross = XMVectorGetY(XMVector3Cross(vSelfDir, vTargetDir));
	//if (0 > fCross)
	//	fAngle = -fAngle;
	//
	//if (fDot > 0.99f) {
	//	m_tRotDir.isLookPlayer = false;
	//	return;
	//}
	//
	//m_pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle) * m_tRotDir.fRotateSpeed);

	XMVECTOR vTargetDir = XMLoadFloat3(&m_tRotDir.vDirToLookCapture);
	XMVECTOR vSelfDir = m_pTransform->Dir(Engine::STATE::LOOK);

	// Y 제거: XZ 평면에서만 회전
	vTargetDir = XMVectorSetY(vTargetDir, 0.f);
	vSelfDir = XMVectorSetY(vSelfDir, 0.f);

	// 0벡터 방지
	if (XMVectorGetX(XMVector3LengthSq(vTargetDir)) < 1e-6f ||
		XMVectorGetX(XMVector3LengthSq(vSelfDir)) < 1e-6f)
		return;

	vTargetDir = XMVector3Normalize(vTargetDir);
	vSelfDir = XMVector3Normalize(vSelfDir);

	float dot = XMVectorGetX(XMVector3Dot(vSelfDir, vTargetDir));
	dot = std::clamp(dot, -1.f, 1.f);

	float crossY = XMVectorGetY(XMVector3Cross(vSelfDir, vTargetDir));

	// signed angle [-pi, pi] (acos + sign보다 안정적)
	float angle = atan2f(crossY, dot);

	// 종료 허용 오차 (예: 0.5도)
	const float eps = XMConvertToRadians(0.5f);
	if (fabsf(angle) < eps)
	{
		m_tRotDir.isLookPlayer = false;
		return;
	}

	//// 이번 프레임 최대 회전량
	//// rotateSpeed가 deg/sec라면:
	//float maxStep = XMConvertToRadians(m_tRotDir.fRotateSpeed) * dt;
	//// rotateSpeed가 rad/sec라면:
	//// float maxStep = m_tRotDir.fRotateSpeed * dt;
	//
	//float step = std::clamp(angle, -maxStep, +maxStep);
	//
	//m_pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), step);

	// angle은 signed angle(라디안)이라고 가정
	float step = angle * m_tRotDir.fRotateSpeed;   // fRotateSpeed = 0~1(혹은 0~5) 같은 민감도

	// 너무 큰 값으로 튀는 거 방지용 상한만 둠(선택)
	float maxStep = XMConvertToRadians(720.f) * dt; // 초당 최대 720도까지만
	step = std::clamp(step, -maxStep, +maxStep);

	m_pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), step);
}

