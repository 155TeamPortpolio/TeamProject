#include "pch.h"
#include "MeleeJaeger_Shield.h"
#include "MeleeJaeger.h"
#include "GameInstance.h"
#include "BattleSystem.h"

/* Component */
#include "Material.h"
#include "StaticModel.h"
#include "ObjectContainer.h"
#include "Collider.h"
#include "Child.h"

CMeleeJaeger_Shield::CMeleeJaeger_Shield()
	: CEnemy()
{
}

CMeleeJaeger_Shield::CMeleeJaeger_Shield(const CMeleeJaeger_Shield& rhg)
	: CEnemy(rhg)
{
}

HRESULT CMeleeJaeger_Shield::Initialize_Prototype()
{
	Add_Component<CObjectContainer>();
	Add_Component<CCollider>();
	Add_Component<CRigidBody>();
	Add_Component<CMaterial>();
	Add_Component<CStaticModel>();

	auto pResourceMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pResourceMgr->Add_ResourcePath("MeleeJaeger_Shield.mat", "../Bin/Resources/Zero/Enemy/MeleeJaeger_Shield/MeleeJaeger_Shield.mat");
	pResourceMgr->Add_ResourcePath("MeleeJaeger_Shield.model", "../Bin/Resources/Zero/Enemy/MeleeJaeger_Shield/MeleeJaeger_Shield.model");

	auto pModel = Get_Component<CStaticModel>();
	pModel->Link_Model(G_GlobalLevelKey, "MeleeJaeger_Shield.model");

	auto pMaterial = Get_Component<CMaterial>();
	pMaterial->Link_Material(G_GlobalLevelKey, "MeleeJaeger_Shield.mat");

	return S_OK;
}

HRESULT CMeleeJaeger_Shield::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	JAEGERSHIELD_DESC* pDesc = static_cast<JAEGERSHIELD_DESC*>(pArg);

	m_pHandBone = pDesc->pHandBone;
	m_pWeaponBone = pDesc->pWeaponBone;
	if (nullptr == pDesc->pWeaponBone)
		return E_FAIL;

	Get_Component<CRigidBody>()->Set_Kinematic(true);
	m_vOffset = { -0.05f, -0.03f, 0.17f };

	return S_OK;
}

void CMeleeJaeger_Shield::Awake()
{
	__super::Awake();
}

void CMeleeJaeger_Shield::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CMeleeJaeger_Shield::Update(_float dt)
{
	if (!m_isFirstCompute)
	{
		dynamic_cast<CMeleeJaeger*>(Get_Component<CChild>()->Get_Parent())->SetIsShield(true);
		m_isFirstCompute = true;
	}
	ComputePosition();
	ComputeRoll(dt);

	__super::Update(dt);
}

void CMeleeJaeger_Shield::Late_Update(_float dt)
{
	Get_Component<CRigidBody>()->Late_Update(dt);
	__super::Late_Update(dt);
}

void CMeleeJaeger_Shield::Render_GUI()
{
	ImGui::PushID(this);
	if (ImGui::TreeNode("Inspector"))
	{
		__super::Render_GUI();
		ImGui::TreePop();
	}

	ImGui::Text("Roll Degree : %.2f", m_fComputeDegree);

	float v[3] = { m_vOffset.x, m_vOffset.y, m_vOffset.z };

	if (ImGui::DragFloat3("##ShieldOffset", v, 0.01f))  // speed = 0.1
	{
		m_vOffset.x = v[0];
		m_vOffset.y = v[1];
		m_vOffset.z = v[2];
	}
	ImGui::DragFloat("##ShieldRoll", &m_fRollDegree, 0.01f);  // speed = 0.1

	ImGui::PopID();
}

void CMeleeJaeger_Shield::OnTriggerEnter(CGameObject* pOther)
{
}

void CMeleeJaeger_Shield::TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName)
{
	__super::TakeDamage(eDamageType, fDamage, charaName);
	m_tStatus.iGroggyValue = 0;

	if (0 >= m_tStatus.iNowHP)
	{
		m_isAlive = false;
		dynamic_cast<CMeleeJaeger*>(Get_Component<CChild>()->Get_Parent())->SetIsShield(false); 
		Get_Component<CCollider>()->Set_CompActive(false);
	}

}

void CMeleeJaeger_Shield::StartRoll(_float fDegree)
{
	m_fRollDegree = fDegree;

	m_isRoll = true;
	//m_isStartRoll = true;
}

void CMeleeJaeger_Shield::ComputePosition()
{
	auto pChildCom = Get_Component<CChild>();
	if (nullptr == pChildCom)
		return;

	auto pParent = pChildCom->Get_Parent();
	_matrix mLocal = XMLoadFloat4x4(m_pTransform->Get_WorldMatrix_Ptr());
	_matrix mWeaponBone = XMLoadFloat4x4(m_pWeaponBone);
	_matrix mParentWorld = XMLoadFloat4x4(pParent->Get_WorldMatrix_Ptr());

	_matrix mResult = mWeaponBone * mParentWorld;

	_vector vPos = mResult.r[3];

	{
		_matrix mWeaponBone = XMLoadFloat4x4(m_pWeaponBone);
		_matrix mHandBone = XMLoadFloat4x4(m_pHandBone);
		_matrix mParentWorld = XMLoadFloat4x4(pParent->Get_WorldMatrix_Ptr());

		_matrix mWeaponW = mWeaponBone * mParentWorld;
		_matrix mHandW = mHandBone * mParentWorld;

		// --- 위치 ---
		XMVECTOR weaponPos = mWeaponW.r[3];
		XMVECTOR handPos = mHandW.r[3];

		// === 본체 Look 가져오기(여기만 네 프로젝트에 맞게) ===
		// 예: pParent가 곧 "본체"면 그대로 사용.
		// 아니면 루트/바디 트랜스폼(혹은 spine/pelvis 본)에서 따로 얻어와야 함.
		_matrix mBodyWorld = mParentWorld;

		const XMVECTOR WORLD_UP = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		// 본체 look(수평)
		XMVECTOR bodyLook = XMVector3Normalize(mParentWorld.r[2]);
		bodyLook = bodyLook - XMVector3Dot(bodyLook, WORLD_UP) * WORLD_UP;
		if (XMVectorGetX(XMVector3LengthSq(bodyLook)) < 1e-6f)
			bodyLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		bodyLook = XMVector3Normalize(bodyLook);

		// hand -> weapon (수평) : Right 힌트
		XMVECTOR rightHint = mResult.r[3] - mHandW.r[3];
		rightHint = rightHint - XMVector3Dot(rightHint, WORLD_UP) * WORLD_UP;

		if (XMVectorGetX(XMVector3LengthSq(rightHint)) < 1e-6f)
			rightHint = XMVector3Normalize(XMVector3Cross(WORLD_UP, bodyLook));
		else
			rightHint = XMVector3Normalize(rightHint);

		if (m_hasPrev)
		{
			// 이전 회전의 월드 Right
			XMVECTOR prevRight = XMVector3Rotate(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_prevQ);
			if (XMVectorGetX(XMVector3Dot(prevRight, rightHint)) < 0.f)
				rightHint = XMVectorNegate(rightHint);
		}

		// Right 힌트로 Look 생성 (LH 기준: right x up = look)
		XMVECTOR look = XMVector3Normalize(XMVector3Cross(rightHint, WORLD_UP));

		// 본체 Look과 반대면 뒤집기(정면 정렬)
		if (XMVectorGetX(XMVector3Dot(look, bodyLook)) < 0.f)
		{
			look = XMVectorNegate(look);
			rightHint = XMVectorNegate(rightHint);
		}

		// 직교화
		XMVECTOR right = XMVector3Normalize(XMVector3Cross(WORLD_UP, look));
		XMVECTOR up = WORLD_UP;
		look = XMVector3Cross(right, up);

		// q 후보 1
		XMMATRIX rotM = XMMatrixIdentity();
		rotM.r[0] = right;
		rotM.r[1] = up;
		rotM.r[2] = look;
		XMVECTOR q1 = XMQuaternionNormalize(XMQuaternionRotationMatrix(rotM));

		XMMATRIX rotFlip = XMMatrixIdentity();
		rotFlip.r[0] = XMVectorNegate(right);
		rotFlip.r[1] = up;
		rotFlip.r[2] = XMVectorNegate(look);
		XMVECTOR q2 = XMQuaternionNormalize(XMQuaternionRotationMatrix(rotFlip));
		
		XMVECTOR q = q1;
		if (m_hasPrev)
		{
			float d1 = fabsf(XMVectorGetX(XMQuaternionDot(m_prevQ, q1)));
			float d2 = fabsf(XMVectorGetX(XMQuaternionDot(m_prevQ, q2)));
			q = (d2 > d1) ? q2 : q1;

			if (XMVectorGetX(XMQuaternionDot(m_prevQ, q)) < 0.f)
				q = XMVectorNegate(q);
		}

		m_pTransform->Set_WorldQuaternion(q);

		// prev 갱신
		m_prevQ = q;
		m_hasPrev = true;


		// 로컬 오프셋 (x=Right, y=Up, z=Look) 라고 가정
		XMVECTOR offLocal = XMVectorSet(m_vOffset.x, m_vOffset.y, m_vOffset.z, 0.f);

		// 최종 회전(q) 기준으로 월드 오프셋 생성
		XMVECTOR offWorld = XMVector3Rotate(offLocal, q);

		// 위치 적용
		weaponPos += offWorld;

		_float3 vResultPos{};
		XMStoreFloat3(&vResultPos, weaponPos);
		m_pTransform->Set_Pos(vResultPos);
	}
}

void CMeleeJaeger_Shield::ComputeRoll(_float dt)
{
	if (false == m_isRoll)
		return;

	_matrix mWorld = XMLoadFloat4x4(m_pTransform->Get_WorldMatrix_Ptr());
	
	_float fRollDegree = m_fRollDegree;

	if (m_isEndRoll)
	{
		m_vEndRollTime.y += dt;
	
		_float t = m_vEndRollTime.y / m_vEndRollTime.x;
		t = clamp(t, 0.f, 1.f);
	
		m_fComputeDegree = fRollDegree = (1 - t) * m_fRollDegree;

		if (m_vEndRollTime.x <= m_vEndRollTime.y)
		{
			m_vEndRollTime.y = 0.f;
			m_isEndRoll = false;
			m_isRoll = false;
		}
	}


	m_pTransform->Rotation(XMVector3Normalize(mWorld.r[2]), XMConvertToRadians(fRollDegree));//-50
}

CMeleeJaeger_Shield* CMeleeJaeger_Shield::Create()
{
	CMeleeJaeger_Shield* instance = new CMeleeJaeger_Shield();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CCyclops_Spit");
	}

	return instance;
}

CGameObject* CMeleeJaeger_Shield::Clone(INIT_DESC* pArg)
{
	CMeleeJaeger_Shield* instance = new CMeleeJaeger_Shield(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CMeleeJaeger_Shield");
	}

	return instance;
}

void CMeleeJaeger_Shield::Free()
{
	__super::Free();
}
