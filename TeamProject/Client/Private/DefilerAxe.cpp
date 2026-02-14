#include "pch.h"
#include "DefilerAxe.h"

#include "BattleSystem.h"
#include "GameInstance.h"

#include "StaticModel.h"
#include "Material.h"
#include "CharacterController.h"
#include "ObjectContainer.h"

#include "Helper_Func.h"
#include "Character.h"
#include "Defiler.h"
#include "Texture.h"
#include "AudioSource.h"
#include "UI_DamageText.h"
#include "UIDirector.h"
#include "DefilerWall.h"

CDefilerAxe::CDefilerAxe()
	: CEnemy()
{
}

CDefilerAxe::CDefilerAxe(const CDefilerAxe& rhs)
	:CEnemy(rhs)
{
}

HRESULT CDefilerAxe::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CStaticModel>()->Link_Model("Zero_Level", "Defile_Axe.model");
	Add_Component<CMaterial>()->Link_Material("Zero_Level", "Defile_Axe.mat");
	Add_Component<CCharacterController>();

	return S_OK;
}

HRESULT CDefilerAxe::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	auto desc = static_cast<DefilerAxeDesc*>(pArg);
	Get_Component<CCharacterController>()->Set_BoundingMinY(1.3f);
	m_pTransform->Set_Look(desc->vLook);
	m_vSlide = desc->vLook;
	m_vSlide = Math::NormalizeSafeXZ(m_vSlide)*5;
	m_BaseRot = m_pTransform->Get_QuaternionRotate(); 
	m_fElapsedTime = 0.f;
	m_tStatus.iNowHP = 100.f;
	return S_OK;
}

void CDefilerAxe::Awake()
{

}

void CDefilerAxe::Priority_Update(_float dt)
{
}

void CDefilerAxe::Update(_float dt)
{

	m_fElapsedTime += dt;
	Get_Component<CCharacterController>()->Update(dt);

	if (!m_bDangle)
	{
		m_vSlide *= expf(-4.f * dt);
		const float length = _vector3(m_vSlide.x, 0.f, m_vSlide.z).Length();
		const float speedRef = 4.0f;
		float speed = length / speedRef;
		speed = Math::ApplyEase(EaseType::OutCubic, speed);

		const float rad = XMConvertToRadians(m_ShakeAmpDeg) * speed;

		float hz = 1.f + 5.f * speed; 
		float phase = m_fElapsedTime * hz * XM_2PI;
		const float angle = -sinf(phase) * rad;

		_quaternion qOffset = _quaternion::CreateFromAxisAngle(_vector3(1.f, 0.f, 0.f), angle);
		_quaternion qShake = qOffset * m_BaseRot;

		const float settleSpeed = 8.f;
		const float t = 1.f - expf(-settleSpeed * dt);
		_quaternion qFinal = _quaternion::Slerp(qShake, m_BaseRot, t);
		m_pTransform->Set_Quaternion(qFinal);
		Get_Component<CCharacterController>()->Move_RootMotion(m_vSlide * dt, {},dt);
		if (rad < XMConvertToRadians(0.2f))
			m_bDangle = true;
	}

	if (m_tStatus.iNowHP <= 0.f)
		SummonWall();
}

void CDefilerAxe::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);
}

void CDefilerAxe::Render_GUI()
{
	__super::Render_GUI();
}

void CDefilerAxe::OnPooledAcquire(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	auto desc = static_cast<DefilerAxeDesc*>(pArg);
	Get_Component<CCharacterController>()->Set_BoundingMinY(1.3f);
	m_pTransform->Set_Look(desc->vLook);
	m_vSlide = desc->vLook;
	m_vSlide = Math::NormalizeSafeXZ(m_vSlide) * 5;
	m_BaseRot = m_pTransform->Get_QuaternionRotate();
	m_fElapsedTime = 0.f;
	Get_Component<CCharacterController>()->Set_CompActive(true);
}

void CDefilerAxe::OnPooledRelease()
{
	Get_Component<CCharacterController>()->Set_CompActive(false);
}


void CDefilerAxe::TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage, CHARACTER charaName)
{
	BattleSystem()->StartGimmick(BATTLE_VFX_TYPE::HIT_NORMAL);
	_float fTakeDamage = fDamage;
	m_tStatus.iNowHP -= fTakeDamage;

	if (0 >= m_tStatus.iNowHP)
		m_tStatus.iNowHP = 0.f;

	DAMAGE_DESC desc{};
	_int damage = Helper::Get_Random_Int(1000, 10000); // юс╫ц

	desc.damage = damage;
	desc.followHandle = Get_Handle();
	desc.followOffset = {0,0,0};
	desc.isEnemy = true;
	desc.charaName = charaName;

	UIDirector()->Request_DamageText(desc);
}

void CDefilerAxe::SummonWall()
{
	string nowLevelKey = LevelManager()->Get_NowLevelKey();
	CDefilerWall::DefilerWallDesc* desc = new CDefilerWall::DefilerWallDesc;
	desc->vLook = Math::NormalizeSafeXZ(m_pTransform->Dir(STATE::LOOK));
	_vector3 pos =  m_pTransform->Get_Pos();
	pos.y = 0;

	auto pWall = Builder::Create_Object({ "Zero_Level","Proto_GameObject_DefilerWall" })
		.Position(pos)
		.Add_ObjDesc(desc)
		.Build("Wall");
	ObjectManager()->Add_Object(pWall, { nowLevelKey,"Enemy_Layer" });
	ObjectManager()->Remove_Object(this);
}


CDefilerAxe* CDefilerAxe::Create()
{
	CDefilerAxe* instance = new CDefilerAxe();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CDefilerAxe");
	}

	return instance;
}

CGameObject* CDefilerAxe::Clone(INIT_DESC* pArg)
{
	CDefilerAxe* instance = new CDefilerAxe(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CDefilerAxe");
	}

	return instance;
}

void CDefilerAxe::Free()
{
	__super::Free();
}

void CDefilerAxe::OnTriggerEnter(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable)
		return;

	else {
		auto pEnemy = dynamic_cast<CCharacter*>(pOther);
		if (nullptr != pEnemy)
		{
			pEnemy->Take_Damage(DAMAGE_TYPE::NORMAL, 10);
			CameraManager()->AddImpact(1, 0);
		}
	}
}