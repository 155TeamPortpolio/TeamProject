#include "pch.h"
#include "Grid.h"
#include "GameInstance.h"
#include "PlaneModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "RayReceiver.h"

HRESULT CGrid::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CPlaneModel>();
	Add_Component<CMaterial>();
	Add_Component<CRayReceiver>();
	auto resMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	resMgr->Add_ResourcePath("TileCell.png", "../bin/Resources/TileCell.png");
	resMgr->Add_ResourcePath("VTX_PlaneGrid.hlsl", "../bin/ShaderFiles/VTX_PlaneGrid.hlsl");

	return S_OK;
}

HRESULT CGrid::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	CPlaneModel* pModel = Get_Component<CPlaneModel>();

	CMaterial* pMaterial = Get_Component<CMaterial>();
	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Grid_Material", "Opaque", CGameInstance::GetInstance()->Get_Device());
	static _float size = 20.f ;
	customInstance->Set_Param("GridSize", { &size, "float", sizeof(size) });

	_uint Index = {};
	pMaterial->Insert_MaterialInstance(customInstance, &Index);
	pModel->Set_RenderType(RENDER_PASS_TYPE::RENDER_OPAQUE);
	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
		MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_PlaneGrid.hlsl");

	customInstance->Get_MaterialData()->Link_Texture(G_GlobalLevelKey, "TileCell.png", TEXTURE_TYPE::DIFFUSE);

	return S_OK;
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