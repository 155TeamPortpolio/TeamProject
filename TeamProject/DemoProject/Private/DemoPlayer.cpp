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
	//Add_Component<CStaticModel>();
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

}

void CDemoPlayer::Priority_Update(_float dt)
{
}

void CDemoPlayer::Update(_float dt)
{
	CCharacterController* pCCT = Get_Component<CCharacterController>();
	_vector3 vMoveDir = XMVectorZero();

	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down(VK_UP))
		vMoveDir += m_pTransform->Dir(STATE::LOOK);
	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down(VK_DOWN))
		vMoveDir -= m_pTransform->Dir(STATE::LOOK);
	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down(VK_RIGHT))
		vMoveDir += m_pTransform->Dir(STATE::RIGHT);
	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down(VK_LEFT))
		vMoveDir -= m_pTransform->Dir(STATE::RIGHT);

	pCCT->Move_Direction(vMoveDir, 5.0f);

	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Down('J'))
		pCCT->Jump(10.f);
	

	if (CGameInstance::GetInstance()->Get_InputDev()->Key_Hold('F'))
	{
		_vector vLook = m_pTransform->Dir(STATE::LOOK);
		pCCT->Shoot_Ray(vLook, 100.f);
	}
	else
	{
		pCCT->Clear_DebugRay();
	}
	
	pCCT->Update(dt);
}

void CDemoPlayer::Late_Update(_float dt)
{
	Get_Component<CCharacterController>()->Late_Update(dt);
}

void CDemoPlayer::OnCollisionEnter()
{
	OutputDebugStringA("CCT Collision Enter!\n");
}

void CDemoPlayer::OnCollisionStay()
{
	OutputDebugStringA("CCT Collision Stay!\n");
}

void CDemoPlayer::OnCollisionExit()
{
	OutputDebugStringA("CCT Collision Exit!\n");
}

void CDemoPlayer::Render_GUI()
{
	__super::Render_GUI();

	if (ImGui::Button("Add")) {
		CGameObject* DemoModel = Builder::Create_Object({ "Demo_Level" ,"Proto_GameObject_DemoPlayer" })
			.Position({ 0,0,0 })
			.Build("Demo_Model");
		Get_Component<CObjectContainer>()->Add_Child(DemoModel, false);
	}
	_bool isLayer = Get_Layer();
	ImGui::Checkbox("InLayer", &isLayer);
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
