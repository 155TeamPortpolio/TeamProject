#include "pch.h"
#include "ScottCar.h"


#include "GameInstance.h"
#include "FieldSystem.h"

//component
#include "SkeletalModel.h"
#include "StaticModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "CharacterController.h"
#include "ObjectContainer.h"

//state
#include "StateMachine.h"

CScottCar::CScottCar()
	:CServiceNpc()
{
}

CScottCar::CScottCar(const CScottCar& rhs)
	:CServiceNpc(rhs)
{
}

void CScottCar::Execute()
{
	UI_DIALOGUE_REQUEST_DESC desc;
	desc.strDialogueID = m_DiagloueData.StartDialogueID;
	desc.iSequenceID = m_iNextSequceID;
	EventSystem()->Broadcast<UI_DIALOGUE_REQUEST_DESC>({ desc });
}

HRESULT CScottCar::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	auto pResource = ResourceManager();
	
	Remove_Component<CSkeletalModel>();
	Add_Component<CStaticModel>();

	Get_Component<CStaticModel>()->Link_Model("MainCity_Level", "Common_Object_MainCity_Car_001.model");
	Get_Component<CMaterial>()->Link_Material("MainCity_Level", "Common_Object_MainCity_Car_001.mat");

	return S_OK;
}

HRESULT CScottCar::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CScottCar::Awake()
{
	Get_Component<CCharacterController>()->Set_CollisionMask(ENUM(COLLISION_GROUP::GROUND));

	m_strAnimName = "";
	m_strName = L"전초기지 차량";

	__super::Awake();

	Add_InteractZone(Get_Position(), _float3(3.9f, 1.f, 0.f), Get_WorldRotation(), _float3{1.3f, 1.f, 2.5f});
}

void CScottCar::Priority_Update(_float dt)
{
	__super::Priority_Update(dt);
}

void CScottCar::Update(_float dt)
{
	CObjectContainer* pObjectContainer = Get_Component<CObjectContainer>();
	if (pObjectContainer) pObjectContainer->UpdateChild(dt);
	Get_Component<CCharacterController>()->Update(dt);
}

void CScottCar::Late_Update(_float dt)
{
	__super::Late_Update(dt);
}

void CScottCar::Success(_uint curSequenceID)
{
	LevelManager()->Request_ChangeLevel("Scott_Level", true);
}

CScottCar* CScottCar::Create()
{
	CScottCar* instance = new CScottCar();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CScottCar");
	}

	return instance;
}

CGameObject* CScottCar::Clone(INIT_DESC* pArg)
{
	CScottCar* instance = new CScottCar(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CScottCar");
	}
	return instance;
}

void CScottCar::Free()
{
	__super::Free();
}
