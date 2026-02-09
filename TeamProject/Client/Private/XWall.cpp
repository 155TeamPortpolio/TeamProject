#include "pch.h"
#include "XWall.h"
#include "GameInstance.h"

#include "Texture.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "InstanceModel.h"
#include "RectModel.h"
#include "BattleSystem.h"
#include "BattlePlayer.h"

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
	Add_Component<CInstanceModel>();
	Add_Component<CMaterial>();
    return S_OK;
}

HRESULT CXWall::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	INSTANCE_INIT_DESC instanceDesc = {};
	instanceDesc.ElementKey = "ClientVTXXWall";
	instanceDesc.ElementCount = VTX_XWALLINSTANCE::iElementCount;
	instanceDesc.instanceStride = sizeof(VTX_XWALLINSTANCE);
	instanceDesc.pElementDesc = VTX_XWALLINSTANCE::Elements;
	instanceDesc.instanceCount = m_vCount.x * m_vCount.y;

	m_InitDescs.push_back(instanceDesc);

	//ResourceManager()->Add_ResourcePath("Eff_Objects_041.png", "../Bin/Resources/Global/Effect/Texture/Diffuse/Eff_Objects_041.png");
	//ResourceManager()->Add_ResourcePath("Rect.model", "../Bin/Resources/Global/Effect/Model/Rect/Rect.model");
	//ResourceManager()->Add_ResourcePath("Rect.mat", "../Bin/Resources/Global/Effect/Model/Rect/Rect.mat");
	//ResourceManager()->Add_ResourcePath("Client_Shader_XWall.hlsl", "../Bin/ShaderFiles/Client_Shader_XWall.hlsl");

	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("XWall", "Default", CGameInstance::GetInstance()->Get_Device());
	customInstance->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);
	customInstance->Set_Blended(true);

	Get_Component<CMaterial>()->Insert_MaterialInstance(customInstance, nullptr);
	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
	{
		MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_XWall.hlsl");
		MaterialDat->Link_Texture(G_GlobalLevelKey, "Eff_Objects_041.png", TEXTURE_TYPE::DIFFUSE);
	}

	Get_Component<CInstanceModel>()->Link_InstanceData(CGameInstance::GetInstance()->Get_Device(), m_InitDescs, G_GlobalLevelKey, "Rect.model");
	Get_Component<CInstanceModel>()->Link_InstanceMeshAll(0);
	Get_Component<CInstanceModel>()->ShadowCast(false);

	for (auto& instance : Get_Component<CMaterial>()->Get_MaterialInstances())
	{
		instance->Set_Param("DiffuseTexture", { ResourceManager()->Load_Texture(G_GlobalLevelKey, "Eff_Objects_041.png")->Get_SRV(), "Texture2D", 0 });
		instance->Override_Pass("Default");
	}

	m_XWall.resize(instanceDesc.instanceCount, { {1,0,0,0},{0,1,0,0 },{0,0,1,0},{0,0,0,1}, {0} });
	m_fBrightness.resize(instanceDesc.instanceCount, 0.f);

    return S_OK;
}

void CXWall::Awake()
{
	const _int countX = static_cast<_int>(m_vCount.x);
	const _int countY = static_cast<_int>(m_vCount.y);

	m_XWall.resize(countX * countY);

	const _float halfX = (m_vCount.x - 1) * m_vOffset.x * 0.5f;
	const _float halfY = (m_vCount.y - 1) * m_vOffset.y * 0.5f;

	for (_int y = 0; y < countY; ++y)
	{
		for (_int x = 0; x < countX; ++x)
		{
			const _int index = y * countX + x;

			_vector4 localPos = {
				x * m_vOffset.x - halfX,
				y * m_vOffset.y - halfY,
				0.f,
				1.f
			};

			m_XWall[index].vTranslation = localPos;
		}
	}
}

void CXWall::Priority_Update(_float dt)
{
}

void CXWall::Update(_float dt)
{
	_vector3 vPlayerPos = CBattleSystem::GetInstance()->GetBattlePlayer()->GetCurCharacterHandle().Get()->Get_WorldPos();

	for (size_t i = 0; i < m_XWall.size(); i++)
	{
		_vector3 PointPos = { m_XWall[i].vTranslation.x, m_XWall[i].vTranslation.y, m_XWall[i].vTranslation.z};

		if (0.5f >= _vector3::Distance(PointPos, vPlayerPos))
			m_fBrightness[i] = 1.f;
		else
			m_fBrightness[i] = 0.5f;
	}

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
		MSG_BOX("Failed to clone : CXWall");
	}

	return pInstance;
}

void CXWall::Free()
{
    __super::Free();
}
