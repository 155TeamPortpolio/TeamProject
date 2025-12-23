#include "pch.h"
#include "ToolGrid.h"
#include "GameInstance.h"
#include "PlaneModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"

CToolGrid::CToolGrid()
	:m_pDevice{ CGameInstance::GetInstance()->Get_Device() }
	, m_pContext{ CGameInstance::GetInstance()->Get_Context() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CToolGrid::CToolGrid(const CToolGrid& rhs)
	:CGameObject(rhs), m_pDevice(rhs.m_pDevice), m_pContext(rhs.m_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CToolGrid::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CPlaneModel>();
	Add_Component<CMaterial>();
	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();

	pRcsMgr->Add_ResourcePath("TileCell.png", "../../DemoResource/TileCell.png");
	pRcsMgr->Add_ResourcePath("VTX_PlaneGrid.hlsl", "../Bin/ShaderFiles/VTX_PlaneGrid.hlsl");

	return S_OK;
}

HRESULT CToolGrid::Initialize(INIT_DESC* pArg)
{

	__super::Initialize(pArg);
	CPlaneModel* pModel = Get_Component<CPlaneModel>();
	CMaterial* pMaterial = Get_Component<CMaterial>();
	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Grid_Material", "Opaque", m_pDevice);

	_uint Index = {};
	pMaterial->Insert_MaterialInstance(customInstance, &Index);
	pModel->Set_RenderType(RENDER_PASS_TYPE::RENDER_OPAQUE);
	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
		MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_PlaneGrid.hlsl");

	customInstance->Get_MaterialData()->Link_Texture(G_GlobalLevelKey, "TileCell.png", TEXTURE_TYPE::DIFFUSE);
	customInstance->Set_Param("GridSize", { &m_fGridSize,"float",sizeof(_float) });
	return S_OK;
}

void CToolGrid::Priority_Update(_float dt)
{
}

void CToolGrid::Update(_float dt)
{
}

void CToolGrid::Late_Update(_float dt)
{
}

void CToolGrid::Render_GUI()
{
	__super::Render_GUI();
}

CToolGrid* CToolGrid::Create()
{
	CToolGrid* instance = new CToolGrid();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CToolGrid");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CToolGrid::Clone(INIT_DESC* pArg)
{
	CToolGrid* instance = new CToolGrid(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CToolGrid");
		Safe_Release(instance);
	}

	return instance;
}

void CToolGrid::Free()
{
	__super::Free();
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}