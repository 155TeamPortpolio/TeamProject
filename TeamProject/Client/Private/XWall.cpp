#include "pch.h"
#include "XWall.h"
#include "VI_InstancePoint.h"
#include "Material.h"
#include "MaterialInstance.h"
CXWall::CXWall()
    :CGameObject()
{
}

CXWall::CXWall(const CXWall& rhs)
    :CGameObject(rhs)
{
}

HRESULT CXWall::Initialize_Prototype()
{
    __super::Initialize_Prototype();
	//Add_Component<CVI_InstancePoint>();
	//Add_Component<CMaterial>();
    return S_OK;
}

HRESULT CXWall::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
	//
	//ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
	//
	//auto pModel = Get_Component<CVI_InstancePoint>();
	//pModel->
	//auto pMaterial = Get_Component<CMaterial>();
	//CMaterialInstance* pMaterialInstance = CMaterialInstance::Create_Handle("Default_Instance", "ScreenEffect", pDevice);
	//pMaterialInstance->Set_Param("ScreenWidth", { &m_fScreenWidth,"float",sizeof(_float) });
	//pMaterialInstance->Set_Param("ScreenHeight", { &m_fScreenHeight,"float",sizeof(_float) });
	//pMaterialInstance->Set_Param("Width", { &m_fWidth,"float",sizeof(_float) });
	//pMaterialInstance->Set_Param("Height", { &m_fHeight,"float",sizeof(_float) });
	//pMaterialInstance->Set_Param("Color", { &m_vColor,"float3",sizeof(_float3) });
	//pMaterialInstance->Set_Param("Alpha", { &m_fAlpha,"float",sizeof(_float) });
	//
	//CMaterialData* pMaterialData = pMaterialInstance->Get_MaterialData();
	//pMaterialData->Link_Shader(G_GlobalLevelKey, "VTX_Point.hlsl");
	//pMaterialData->Link_Texture(G_GlobalLevelKey, "attack_sign.png", TEXTURE_TYPE::DIFFUSE);
	//
	//pMaterial->Insert_MaterialInstance(pMaterialInstance, nullptr);

    return S_OK;
}

void CXWall::Priority_Update(_float dt)
{
}

void CXWall::Update(_float dt)
{
}

void CXWall::Late_Update(_float dt)
{
}

CXWall* CXWall::Create()
{
	CXWall* pInstance = new CXWall();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to create : CXWall");
	}

	return pInstance;
}

CGameObject* CXWall::Clone(INIT_DESC* pArg)
{
	CXWall* pInstance = new CXWall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to clone : CAttackSign");
	}

	return pInstance;
}

void CXWall::Free()
{
    __super::Free();
}
