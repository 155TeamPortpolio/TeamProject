#include "pch.h"
#include "ZeroPortal.h"
#include "LevelMgr.h"
#include "GameInstance.h"

//Components
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "PlaneModel.h"
#include "EventListener.h"
#include "Stage.h"

CZeroPortal::CZeroPortal()
	: CInteractable()
{
}

CZeroPortal::CZeroPortal(const CZeroPortal& rhs)
	: CInteractable(rhs)
{
}

HRESULT CZeroPortal::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CEventListener>();
	Add_Component<CPlaneModel>();
	Add_Component<CMaterial>();

	ResourceManager()->Add_ResourcePath("Eff_Objects_048.png", "../Bin/Resources/Effect/Texture/Eff_Objects_048.png");
	ResourceManager()->Add_ResourcePath("Eff_Noise_092.png", "../Bin/Resources/Effect/Texture/Eff_Noise_092.png");
	ResourceManager()->Add_ResourcePath("Eff_Noise_097_LYX_01.png", "../Bin/Resources/Effect/Texture/Eff_Noise_097_LYX_01.png");

	auto pModel = Get_Component<CPlaneModel>();
	pModel->Link_Model(G_GlobalLevelKey, "Engine_Default_Rect");
	CMaterial* pMaterial = Get_Component<CMaterial>();

	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Rect_Effect_Base", "Opaque", pDevice);
	pMaterial->Insert_MaterialInstance(customInstance, nullptr);
	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
		MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_Portal.hlsl")  ;
	customInstance->Get_MaterialData()->Link_Texture(G_GlobalLevelKey, "Eff_Objects_048.png", TEXTURE_TYPE::DIFFUSE);
	customInstance->Get_MaterialData()->Link_Texture(G_GlobalLevelKey, "Eff_Noise_092.png", TEXTURE_TYPE::NOISE);
	customInstance->Get_MaterialData()->Link_Texture(G_GlobalLevelKey, "Eff_Noise_097_LYX_01.png", TEXTURE_TYPE::AMBIENT);

	m_eRenderLayer = RENDER_LAYER::None;
	return S_OK;
}

HRESULT CZeroPortal::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Get_Component<CEventListener>()->Add_Listener<STAGE_CHANGED_DESC>([&](STAGE_CHANGED_DESC desc)
		{
			m_pTargetStage = desc.pStage;
			m_eRenderLayer = RENDER_LAYER::Default;
		});

	return S_OK;
}

void CZeroPortal::Awake()
{
	auto pModel = Get_Component<CPlaneModel>();
	pModel->ShadowCast(false);

	auto pMaterial = Get_Component<CMaterial>();
	auto MaterialInstances = pMaterial->Get_MaterialInstances();
	for (auto& Instance : MaterialInstances)
	{
		pMaterial->Add_MaterialData(Instance, "g_Time", { &m_Time, "float", sizeof(_float) });
	}
}

void CZeroPortal::Priority_Update(_float dt)
{
}

void CZeroPortal::Update(_float dt)
{
	m_Time += dt*2;
	Get_Component<CCollider>()->Update(dt);

	if (m_bIsInteractable) {
		//m_pTransform->AddScale({ sinf(XMConvertToRadians(m_Time)),sinf(XMConvertToRadians(m_Time))*2,dt});
		Interact();
	}
}

void CZeroPortal::Late_Update(_float dt)
{
}

void CZeroPortal::OnTriggerEnter(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;

	m_bIsInteractable = true;
}

void CZeroPortal::OnTriggerStay(CGameObject* pOher)
{

}

void CZeroPortal::OnTriggerExit(CGameObject* pOther)
{
	auto pCollidable = pOther->Get_Component<ICollidable>();
	if (pCollidable && (pCollidable->Get_Group() != COLLISION_GROUP::PLAYER))
		return;

	m_bIsInteractable = false;
}

void CZeroPortal::Interact()
{
	if (!m_bIsInteractable)
		return;

	if (InputDevice()->Key_Tap('F')) {
		m_pTargetStage->StageChangeOn(CZero_Level::StageType::Boss, 0);
		m_bIsInteractable = false;
	}
}

CZeroPortal* CZeroPortal::Create()
{
	CZeroPortal* Instance = new CZeroPortal();
	if (FAILED(Instance->Initialize_Prototype()))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

CGameObject* CZeroPortal::Clone(INIT_DESC* pArg)
{
	CZeroPortal* Instance = new CZeroPortal(*this);
	if (FAILED(Instance->Initialize(pArg)))
	{
		Safe_Release(Instance);
		return nullptr;
	}
	return Instance;
}

void CZeroPortal::Free()
{
	__super::Free();
}


