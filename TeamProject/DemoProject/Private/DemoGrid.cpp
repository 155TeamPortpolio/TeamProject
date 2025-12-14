#include "pch.h"
#include "DemoGrid.h"
#include "GameInstance.h"
#include "PlaneModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"

CDemoGrid::CDemoGrid()
	:m_pDevice{ CGameInstance::GetInstance()->Get_Device() }
	, m_pContext{ CGameInstance::GetInstance()->Get_Context() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CDemoGrid::CDemoGrid(const CDemoGrid& rhs)
	:CGameObject(rhs), m_pDevice(rhs.m_pDevice), m_pContext(rhs.m_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CDemoGrid::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CPlaneModel>();
	Add_Component<CMaterial>();
	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();

	pRcsMgr->Add_ResourcePath("TileCell.png","../../DemoResource/TileCell.png");
	pRcsMgr->Add_ResourcePath("VTX_PlaneGrid.hlsl", "../Bin/ShaderFiles/VTX_PlaneGrid.hlsl");

	return S_OK;
}

HRESULT CDemoGrid::Initialize(INIT_DESC* pArg)
{

	__super::Initialize(pArg);
	CPlaneModel* pModel = Get_Component<CPlaneModel>();
	CMaterial* pMaterial = Get_Component<CMaterial>();
	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Grid_Material", "Opaque", m_pDevice);

	_uint Index = {};
	pMaterial->Insert_MaterialInstance(customInstance, &Index);
	pModel->Set_RenderType(RENDER_PASS_TYPE::PRIORITY);
	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
		MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_PlaneGrid.hlsl");

	customInstance->Get_MaterialData()->Link_Texture(G_GlobalLevelKey, "TileCell.png", TEXTURE_TYPE::DIFFUSE);
	return S_OK;
}

void CDemoGrid::Priority_Update(_float dt)
{
}

void CDemoGrid::Update(_float dt)
{
}

void CDemoGrid::Late_Update(_float dt)
{
}

void CDemoGrid::Render_GUI()
{
	__super::Render_GUI();
}

CDemoGrid* CDemoGrid::Create()
{
	CDemoGrid* instance = new CDemoGrid();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CDemoGrid");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CDemoGrid::Clone(INIT_DESC* pArg)
{
	CDemoGrid* instance = new CDemoGrid(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CDemoGrid");
		Safe_Release(instance);
	}

	return instance;
}

void CDemoGrid::Free()
{
	__super::Free();
}