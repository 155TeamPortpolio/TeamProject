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
}

void CEnemyNormal::RotateToDir(_float dt)
{
	if (false == m_tRotDir.isLookPlayer)
		return;

	_vector vTargetDir = XMLoadFloat3(&m_tRotDir.vDirToLookCapture);
	_vector	vSelfDir = m_pTransform->Dir(Engine::STATE::LOOK);
	vTargetDir = XMVector3Normalize(vTargetDir);
	vSelfDir = XMVector3Normalize(vSelfDir);

	_float fDot = XMVectorGetX(XMVector3Dot(vSelfDir, vTargetDir));
	fDot = max(-1, min(1.f, fDot));
	_float fAngle = acosf(fDot);

	_float fCross = XMVectorGetY(XMVector3Cross(vSelfDir, vTargetDir));
	if (0 > fCross)
		fAngle = -fAngle;

	if (fDot > 0.99f) {
		m_tRotDir.isLookPlayer = false;
		return;
	}

	m_pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle) * m_tRotDir.fRotateSpeed);
}

