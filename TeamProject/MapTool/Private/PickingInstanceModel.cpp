#include "pch.h"
#include "PickingInstanceModel.h"
#include "InstanceModel.h"
#include "RectModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"

#include "GameInstance.h"

CPickingInstanceModel::CPickingInstanceModel()
{
}

CPickingInstanceModel::CPickingInstanceModel(const CPickingInstanceModel& rhs)
	:CGameObject(rhs)
{
}

HRESULT CPickingInstanceModel::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CRectModel>();
	Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CPickingInstanceModel::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	INSTANCE_INIT_DESC instanceDesc = {};
	instanceDesc.ElementKey = "ClientParticeInstance";
	instanceDesc.ElementCount = VTX_PARTICLEINSTANCE::iElementCount;
	instanceDesc.instanceStride = sizeof(INSTANCE_PARTICLE);
	instanceDesc.pElementDesc = VTX_PARTICLEINSTANCE::Elements;
	instanceDesc.instanceCount = 3000; // InstanceMaxCount

	m_InitDescs.push_back(instanceDesc);
	Get_Component<CMaterial>()->Link_Material("GamePlay_Level", "leaf.mat");
	Get_Component<CPickingInstanceModel>()->Link_InstanceData(CGameInstance::GetInstance()->Get_Device(),
	m_InitDescs, "GamePlay_Level", "leaf.model");
	
	Get_Component<CPickingInstanceModel>()->Link_InstanceMeshAll(0);
	
	Get_Component<CPickingInstanceModel>()->ShadowCast(true);
	
	
	m_Particle.resize(m_ParicleCount, { {1,0,0,0},{0,1,0,0 },{0,0,1,0},{0,0,0,1},{0.3,0.4,0,1},{0,2} });
	m_vVelocities.resize(m_ParicleCount, { 0,0,0 });

	//ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
	//CMaterial* pMaterial = Get_Component<CMaterial>();
	////쓰고싶은이름
	//CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("Material_Test", "Opaque", pDevice);
	//pMaterial->Insert_MaterialInstance(customInstance, nullptr);
	//auto MaterialDat = customInstance->Get_MaterialData();
	//if (MaterialDat)
	//	MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_TexPos.hlsl");
	//if (FAILED(customInstance->Get_MaterialData()->Link_Texture(G_GlobalLevelKey, "Test.dds", TEXTURE_TYPE::DIFFUSE)))
	//	return;


	return S_OK;
}

void CPickingInstanceModel::Awake()
{
	//Get_Component<CModel>()->Link_Model("Demo_Level", "");



}

void CPickingInstanceModel::Priority_Update(_float dt)
{
}

void CPickingInstanceModel::Update(_float dt)
{
}

void CPickingInstanceModel::Late_Update(_float dt)
{
}

void CPickingInstanceModel::Render_GUI()
{
	__super::Render_GUI();
}

CPickingInstanceModel* CPickingInstanceModel::Create()
{
	CPickingInstanceModel* instance = new CPickingInstanceModel();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CPickingInstanceModel");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CPickingInstanceModel::Clone(INIT_DESC* pArg)
{
	CPickingInstanceModel* instance = new CPickingInstanceModel(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CPickingInstanceModel");
		Safe_Release(instance);
	}

	return instance;
}

void CPickingInstanceModel::Free()
{
	__super::Free();
}


//HRESULT CFlowerEffect::Initialize(INIT_DESC* pArg)
//{
//	__super::Initialize(pArg);
//
//	m_ParicleCount = 10;
//	INSTANCE_INIT_DESC instanceDesc = {};
//	instanceDesc.ElementKey = "ClientParticeInstance";
//	instanceDesc.ElementCount = VTX_PARTICLEINSTANCE::iElementCount;
//	instanceDesc.instanceStride = sizeof(INSTANCE_PARTICLE);
//	instanceDesc.pElementDesc = VTX_PARTICLEINSTANCE::Elements;
//	instanceDesc.instanceCount = m_ParicleCount;//300
//
//	m_InitDescs.push_back(instanceDesc);
//	Get_Component<CMaterial>()->Link_Material("GamePlay_Level", "leaf.mat");
//	Get_Component<CPickingInstanceModel>()->Link_InstanceData(CGameInstance::GetInstance()->Get_Device(),
//		m_InitDescs, "GamePlay_Level", "leaf.model");
//
//	Get_Component<CPickingInstanceModel>()->Link_InstanceMeshAll(0);
//
//	Get_Component<CPickingInstanceModel>()->ShadowCast(true);
//
//
//	m_Particle.resize(m_ParicleCount, { {1,0,0,0},{0,1,0,0 },{0,0,1,0},{0,0,0,1},{0.3,0.4,0,1},{0,2} });
//	m_vVelocities.resize(m_ParicleCount, { 0,0,0 });
//
//	return S_OK;
//}
//
//void CFlowerEffect::Priority_Update(_float dt)
//{
//
//}
//
//void CFlowerEffect::Update(_float dt)
//{
//	m_fLifeTime += dt;
//
//	for (size_t i = 0; i < m_ParicleCount; i++)
//	{
//		m_vVelocities[i].y -= 2.8f * dt;
//		m_vVelocities[i].x -= 2.8fdt;
//		m_vVelocities[i].z -= 2.8fdt;
//		m_Particle[i].vLifetime.x += dt;
//
//		_float vx = m_vVelocities[i].x * (cosf(m_Particle[i].vLifetime.x * 3));
//		_float vz = m_vVelocities[i].z * (sinf(m_Particle[i].vLifetime.x * 3));
//		m_Particle[i].vTranslation.x += vxdt;
//		m_Particle[i].vTranslation.y += m_vVelocities[i].y 10 * dt;
//		m_Particle[i].vTranslation.z += vzdt;
//		m_Particle[i].vColor.z = atan2(vx, vz);
//	}
//
//	ID3D11DeviceContext pContext = CGameInstance::GetInstance()->Get_Context();
//	Get_Component<CPickingInstanceModel>()->Update_Instance(pContext, m_Particle.data(), 0, static_cast<_uint>(m_Particle.size()));//11121314
//
//}