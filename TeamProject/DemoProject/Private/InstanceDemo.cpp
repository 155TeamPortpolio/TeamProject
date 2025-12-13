#include "pch.h"
#include "InstanceDemo.h"
#include "GameInstance.h"
#include "IResourceService.h"

#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "InstanceModel.h"

CInstanceDemo::CInstanceDemo()
{
}

CInstanceDemo::CInstanceDemo(const CInstanceDemo& rhs)
	:CGameObject(rhs)
{
}

HRESULT CInstanceDemo::Initialize_Prototype()
{
	Add_Component<CInstanceModel>();
	Add_Component<CMaterial>();
    return S_OK;
}

HRESULT CInstanceDemo::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	m_ParicleCount = 10;
	INSTANCE_INIT_DESC instanceDesc = {};
	instanceDesc.ElementKey = "DemoParticeInstance";
	instanceDesc.ElementCount = VTX_PARTICLEINSTANCE::iElementCount;
	instanceDesc.instanceStride = sizeof(INSTANCE_PARTICLE);
	instanceDesc.pElementDesc = VTX_PARTICLEINSTANCE::Elements;
	instanceDesc.instanceCount = m_ParicleCount;

	m_InitDescs.push_back(instanceDesc);
	Get_Component<CMaterial>()->Link_Material("Demo_Level", "Bangboo_Sharkboo_NPC (merge).mat");
	Get_Component<CInstanceModel>()->Link_InstanceData(CGameInstance::GetInstance()->Get_Device(),
		m_InitDescs, "Demo_Level", "Bangboo_Sharkboo_NPC (merge).model");

	Get_Component<CInstanceModel>()->Link_InstanceMeshAll(0);

	m_Particle.resize(m_ParicleCount, { {1,0,0,0},{0,1,0,0 },{0,0,1,0},{10,10,0,1}});
	m_vVelocities.resize(m_ParicleCount, { 0,0,0 });

	return S_OK;
}

void CInstanceDemo::Priority_Update(_float dt)
{
}

void CInstanceDemo::Update(_float dt)
{
	ID3D11DeviceContext* pContext = CGameInstance::GetInstance()->Get_Context();
	Get_Component<CInstanceModel>()->Update_Instance(pContext, m_Particle.data(),0, static_cast<_uint>(m_Particle.size()));
}

void CInstanceDemo::Late_Update(_float dt)
{
}

void CInstanceDemo::Render_GUI()
{
	__super::Render_GUI();
}

CInstanceDemo* CInstanceDemo::Create()
{
	CInstanceDemo* instance = new CInstanceDemo();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CInstanceDemo");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CInstanceDemo::Clone(INIT_DESC* pArg)
{
	CInstanceDemo* instance = new CInstanceDemo(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CInstanceDemo");
		Safe_Release(instance);
	}

	return instance;
}

void CInstanceDemo::Free()
{
	__super::Free();
}