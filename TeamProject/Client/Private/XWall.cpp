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

	if (XWALL_DESC* pDesc = dynamic_cast<XWALL_DESC*>(pArg)) {
		m_vCount = pDesc->vCount;
		m_vOffset = pDesc->vOffset;
	}

	INSTANCE_INIT_DESC instanceDesc = {};
	instanceDesc.ElementKey = "ClientVTXXWall";
	instanceDesc.ElementCount = VTX_XWALLINSTANCE::iElementCount;
	instanceDesc.instanceStride = sizeof(INSTANCE_XWALL);
	instanceDesc.pElementDesc = VTX_XWALLINSTANCE::Elements;
	instanceDesc.instanceCount = m_vCount.x * m_vCount.y;

	m_InitDescs.push_back(instanceDesc);

	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("XWall", "Default", CGameInstance::GetInstance()->Get_Device());
	customInstance->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);
	customInstance->Set_Blended(false);

	Get_Component<CMaterial>()->Insert_MaterialInstance(customInstance, nullptr);
	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
	{
		MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_XWall.hlsl");
		MaterialDat->Link_Texture(G_GlobalLevelKey, "Eff_Objects_041.png", TEXTURE_TYPE::DIFFUSE);
	}

	Get_Component<CInstanceModel>()->Link_InstanceData(CGameInstance::GetInstance()->Get_Device(),
		m_InitDescs, G_GlobalLevelKey, "Rect.model");
	Get_Component<CInstanceModel>()->Link_InstanceMeshAll(0);
	Get_Component<CInstanceModel>()->ShadowCast(false);

	for (auto& instance : Get_Component<CMaterial>()->Get_MaterialInstances())
	{
		auto tex = ResourceManager()->Load_Texture(G_GlobalLevelKey, "Eff_Objects_041.png");
		if (!tex) continue;
		instance->Set_Param("DiffuseTexture", { tex->Get_SRV(), "Texture2D", 0 });
		instance->Override_Pass("Default");
	}
	
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

void CXWall::Render_GUI()
{
	__super::Render_GUI();

	ImGui::Begin("XWall Debug");
	for (int i = 0; i < m_XWall.size(); i++) {
		const auto& inst = m_XWall[i];

		ImGui::Text(
			"%d : Pos(%.2f, %.2f, %.2f) Brightness %.2f",
			i,
			inst.vTranslation.x,
			inst.vTranslation.y,
			inst.vTranslation.z,
			inst.vBrightness
		);
	}

	ImGui::End();
}

void CXWall::Priority_Update(_float dt)
{
}

void CXWall::Update(_float dt)
{
	_vector3 vPlayerPos = CBattleSystem::GetInstance()->GetBattlePlayer()->GetCurCharacterHandle().Get()->Get_WorldPos();

	const _float fMin = 1.5f;
	const _float fMax = 3.0f;

	for (size_t i = 0; i < m_XWall.size(); i++)
	{
		_vector3 vPointPos = { m_XWall[i].vTranslation.x, m_XWall[i].vTranslation.y, m_XWall[i].vTranslation.z};
		_vector3 vWorldPoint = XMVector3TransformCoord(vPointPos, static_cast<Matrix>(m_pTransform->Get_WorldMatrix()));

		float fDist = _vector3::Distance(vWorldPoint, vPlayerPos);
	
		float t = clamp((fDist - fMin) / (fMax - fMin), 0.f, 1.f);
		m_XWall[i].vBrightness = 1.f - t;
	}

	Get_Component<CInstanceModel>()->Update_Instance(CGameInstance::GetInstance()->Get_Context(), m_XWall.data(), 0, static_cast<_uint>(m_XWall.size()));
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
