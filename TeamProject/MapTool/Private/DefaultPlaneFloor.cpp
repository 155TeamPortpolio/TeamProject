#include "pch.h"
#include "DefaultPlaneFloor.h"
#include "StaticModel.h"
#include "RectModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"

#include "GameInstance.h"

CDefaultPlaneFloor::CDefaultPlaneFloor()
{
}

CDefaultPlaneFloor::CDefaultPlaneFloor(const CDefaultPlaneFloor& rhs)
	:CGameObject(rhs)
{
}

HRESULT CDefaultPlaneFloor::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CRectModel>();
	Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CDefaultPlaneFloor::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
	CMaterial* pMaterial = Get_Component<CMaterial>();
	//쓰고싶은이름
	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Material_Test", "Opaque", pDevice);
	pMaterial->Insert_MaterialInstance(customInstance, nullptr);
	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
		MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_TexPos.hlsl");
	if (FAILED(customInstance->Get_MaterialData()->Link_Texture(G_GlobalLevelKey, "Test.dds", TEXTURE_TYPE::DIFFUSE)))
		return E_FAIL;


	return S_OK;
}

void CDefaultPlaneFloor::Awake()
{
	//Get_Component<CModel>()->Link_Model("Demo_Level", "");



}

void CDefaultPlaneFloor::Priority_Update(_float dt)
{
}

void CDefaultPlaneFloor::Update(_float dt)
{
}

void CDefaultPlaneFloor::Late_Update(_float dt)
{
}

void CDefaultPlaneFloor::Render_GUI()
{
	__super::Render_GUI();
}

CDefaultPlaneFloor* CDefaultPlaneFloor::Create()
{
	CDefaultPlaneFloor* instance = new CDefaultPlaneFloor();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CDefaultPlaneFloor");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CDefaultPlaneFloor::Clone(INIT_DESC* pArg)
{
	CDefaultPlaneFloor* instance = new CDefaultPlaneFloor(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CDefaultPlaneFloor");
		Safe_Release(instance);
	}

	return instance;
}

void CDefaultPlaneFloor::Free()
{
	__super::Free();
}