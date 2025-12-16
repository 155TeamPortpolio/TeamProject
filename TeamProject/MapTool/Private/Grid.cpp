#include "pch.h"
#include "Grid.h"
#include "GameInstance.h"
#include "PlaneModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "RayReceiver.h"

CGrid::CGrid()
{
}

CGrid::CGrid(const CGrid& rhs)
	:CGameObject(rhs)
{
}

HRESULT CGrid::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CPlaneModel>();
	Add_Component<CMaterial>();
	Add_Component<CRayReceiver>();

	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pRcsMgr->Add_ResourcePath("TileCell.png", "../Bin/Resources/TileCell.png");
	pRcsMgr->Add_ResourcePath("VTX_PlaneGrid.hlsl",  "../Bin/ShaderFiles/VTX_PlaneGrid.hlsl");

	return S_OK;
}

HRESULT CGrid::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	/* 모델 셋팅 */
	CPlaneModel* pModel = Get_Component<CPlaneModel>();

	/* 머테리얼 셋팅 */
	CMaterial* pMaterial = Get_Component<CMaterial>();
	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Grid_Material", "Opaque", CGameInstance::GetInstance()->Get_Device());

	_uint Index = {};
	pMaterial->Insert_MaterialInstance(customInstance, &Index);
	pModel->Set_RenderType(RENDER_PASS_TYPE::PRIORITY);
	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
		MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_PlaneGrid.hlsl");

	/* 텍스쳐 셋팅 */
	customInstance->Get_MaterialData()->Link_Texture(G_GlobalLevelKey, "TileCell.png", TEXTURE_TYPE::DIFFUSE);
	
	return S_OK;
}

void CGrid::Priority_Update(_float dt)
{
}

void CGrid::Update(_float dt)
{
}

void CGrid::Late_Update(_float dt)
{
}

void CGrid::Render_GUI()
{
	__super::Render_GUI();
}

CGrid* CGrid::Create()
{
	CGrid* instance = new CGrid();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CGrid");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CGrid::Clone(INIT_DESC* pArg)
{
	CGrid* instance = new CGrid(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CGrid");
		Safe_Release(instance);
	}

	return instance;
}

void CGrid::Free()
{
	__super::Free();
}