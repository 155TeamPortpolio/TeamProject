#include "pch.h"
#include "HealKit.h"
#include "LevelMgr.h"
#include "GameInstance.h"
#include "Material.h"
#include "StaticModel.h"
#include "ObjectContainer.h"
#include "Character.h"
#include "BattleSystem.h"
#include "BattlePlayer.h"
#include "AudioSource.h"

#include "KitObject.h"

CHealKit::CHealKit()
	: CInteractable()
{
}

CHealKit::CHealKit(const CHealKit& rhs)
	: CInteractable(rhs)
{
}

HRESULT CHealKit::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();
	Add_Component<CObjectContainer>();
	Add_Component<CAudioSource>();

	Get_Component<CAudioSource>()->SoundFolder("Zero_Level", "../Bin/Resources/Zero/Interactable/HealKit/Sound");

	PrototypeManager()->Add_ProtoType("Zero_Level", "Kit_Child", CKitObject::Create());
	return S_OK;
}

HRESULT CHealKit::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	auto* pDesc = static_cast<HEALKIT_DESC*>(pArg);

	m_eItemType = pDesc->eItemType;

	if (m_eItemType == ITEMTYPE::END)
		return E_FAIL;
	
	Get_Component<CStaticModel>()->Link_Model("Zero_Level", "Device_Prop_ItemStand_02.model");
	Get_Component<CMaterial>()->Link_Material("Zero_Level", "Device_Prop_ItemStand_02.mat");
	Setting_Child();

	return S_OK;
}

void CHealKit::Awake()
{
	__super::Awake();
}

void CHealKit::Priority_Update(_float dt)
{
	if (m_bActiveItem)
		return;

	m_fElapsedTime += dt;

	if (m_fResetTimer <= m_fElapsedTime) {
		m_bActiveItem = true;
		m_fElapsedTime = 0.f;

		for (auto& Child : Get_Children())
			Child->SetRenderLayer(RENDER_LAYER::Default);
	}

}

void CHealKit::Update(_float dt)
{
	Get_Component<CCollider>()->Update(dt);
	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CHealKit::Late_Update(_float dt)
{
}

void CHealKit::OnTriggerEnter(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;

	if (!m_bActiveItem) return;

	m_bActiveItem = false;
	m_fElapsedTime = 0.f;

	for (auto& Child : Get_Children())
		Child->SetRenderLayer(RENDER_LAYER::None);

	switch (m_eItemType)
	{
	case Client::CHealKit::ITEMTYPE::HP:		Recovery_Health();	break;
	case Client::CHealKit::ITEMTYPE::ENERGY:	Recovery_Energy();	break;
	case Client::CHealKit::ITEMTYPE::END:							break;
	default:														break;
	}

	_int RandSound = Helper::Get_Random_Int(1, 4);
	string Sound = "HealPack" + to_string(RandSound);
	Get_Component<CAudioSource>()->Slot(Sound)
		.Attribute3D(false)
		.Loop(false)
		.Volume(0.4f)
		.Play();
}

void CHealKit::OnTriggerStay(CGameObject* pOher)
{
	
}

void CHealKit::OnTriggerExit(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;
}

void CHealKit::Recovery_Health()
{
	BattleSystem()->GetBattlePlayer()->Recover_HP();
}

void CHealKit::Recovery_Energy()
{
	BattleSystem()->GetBattlePlayer()->Recover_Energy();
	BattleSystem()->GetBattlePlayer()->Recover_Decibel();
}

void CHealKit::Setting_Child()
{
	CKitObject::KIT_DESC* KitDesc = new CKitObject::KIT_DESC;
	KitDesc->iItemType = ENUM(m_eItemType);
	CGameObject* Object = Builder::Create_Object({ "Zero_Level",  "Kit_Child" })
		.Add_ObjDesc(KitDesc)
		.Position(_vector3{ 0.f, 0.25f, 0.f })
		.Build("Kit_Child");

	Add_Component<CObjectContainer>()->Add_Child(Object, true);
}

CHealKit* CHealKit::Create()
{
	CHealKit* Instance = new CHealKit();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CHealKit::Clone(INIT_DESC* pArg)
{
	CHealKit* Instance = new CHealKit(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CHealKit::Free()
{
	__super::Free();
}


