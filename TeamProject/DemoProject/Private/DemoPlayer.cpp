#include "pch.h"
#include "DemoPlayer.h"
#include "GameInstance.h"
#include "ObjectContainer.h"
#include "CharacterController.h"
#include "StaticModel.h"

CDemoPlayer::CDemoPlayer()
{
}

CDemoPlayer::CDemoPlayer(const CDemoPlayer& rhs)
	:CGameObject(rhs)
{
}

HRESULT CDemoPlayer::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CObjectContainer>();
	Add_Component<CCharacterController>();
	Add_Component<CStaticModel>();


	return S_OK;
}

HRESULT CDemoPlayer::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	GAMEOBJECT_DESC* pObjDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	return S_OK;
}

void CDemoPlayer::Awake()
{
	CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath("", "");
	Get_Component<CModel>()->Link_Model("Physics_Level", "../../DemoResources/static/Zero_Vehicle_Bus_01.model");
}

void CDemoPlayer::Priority_Update(_float dt)
{
}

void CDemoPlayer::Update(_float dt)
{
	CCharacterController* pCCT = Get_Component<CCharacterController>();
	_vector vMoveDir = XMVectorZero();

	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down(VK_UP))
		vMoveDir += m_pTransform->Dir(STATE::LOOK);
	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down(VK_DOWN))
		vMoveDir -= m_pTransform->Dir(STATE::LOOK);
	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down(VK_RIGHT))
		vMoveDir += m_pTransform->Dir(STATE::RIGHT);
	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down(VK_LEFT))
		vMoveDir -= m_pTransform->Dir(STATE::RIGHT);

	if (XMVector3Length(vMoveDir).m128_f32[0] > 0.f)
		vMoveDir = XMVector3Normalize(vMoveDir);

	_float fSpeed = 5.0f;
	pCCT->Set_PlanarVelocity(vMoveDir * fSpeed);

	if (pCCT->Is_Grounded()) // 땅에 있을 때만 점프 가능
	{
		if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('J'))
		{
			pCCT->Jump(10.0f);
		}
	}

	pCCT->Update(dt);
}

void CDemoPlayer::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);
}

void CDemoPlayer::OnCollisionEnter()
{
}

void CDemoPlayer::OnCollisionStay()
{
}

void CDemoPlayer::OnCollisionExit()
{
}

void CDemoPlayer::Render_GUI()
{
}

CDemoPlayer* CDemoPlayer::Create()
{
	CDemoPlayer* instance = new CDemoPlayer();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CDemoPlayer");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CDemoPlayer::Clone(INIT_DESC* pArg)
{
	CDemoPlayer* instance = new CDemoPlayer(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CDemoPlayer");
		Safe_Release(instance);
	}

	return instance;
}

void CDemoPlayer::Free()
{
	__super::Free();
}
