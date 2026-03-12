#include "pch.h"
#include "BinaryModel.h"
#include "AI_STModel.h"
#include "AI_SKModel.h"
#include "SkeletalModel.h"
#include "StaticModel.h"
#include "Material.h"
#include "Helper_Func.h"
#include "GameInstance.h"
#include "AI_Material.h"
#include "AIMaterial.h"
#include "AIAnimator3D.h"
#include "AIModelData.h"
#include "DebugRender.h"
#include "Level.h"

CBinaryModel::CBinaryModel()
{
}

CBinaryModel::CBinaryModel(const CBinaryModel& rhs)
	:CGameObject(rhs) 
{
}

HRESULT CBinaryModel::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CBinaryModel::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CBinaryModel::Awake()
{
	Add_Component<CDebugRender>();
}

void CBinaryModel::Priority_Update(_float dt)
{
	if (InputDevice()->Key_Tap('T')) {
		Test();
	}

	if (isTesting) {
		time += dt;
	}
}

void CBinaryModel::Update(_float dt)
{
	if(nullptr != Get_Component<CAnimator3D>())
		Get_Component<CAnimator3D>()->Update_Animation(dt);
}

void CBinaryModel::Late_Update(_float dt)
{
}

void CBinaryModel::Render_GUI()
{
	bool alive = Is_Alive();
	if (ImGui::Checkbox("Alive", &alive))
		Set_Alive(alive);

	if(ImGui::Button("Load Model")) {
		string path = Helper::OpenFile_Dialogue();
		filesystem::path file(path);
		string fileName = file.filename().string();
		CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(fileName, path);
		_bool isSkinned = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_ModelData(m_LevelTag, fileName)->isSkinned();
		if (isSkinned)
			Add_Component<CSkeletalModel>()->Link_Model(m_LevelTag, fileName);
		else
			Add_Component<CStaticModel>()->Link_Model(m_LevelTag, fileName);
	}

	if(ImGui::Button("Load Material")) {
		string path = Helper::OpenFile_Dialogue();
		filesystem::path file(path);
		string fileName = file.filename().string();
		CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(fileName, path);
		Add_Component<CMaterial>()->Link_Material(m_LevelTag, fileName);
	}
	__super::Render_GUI();
}


CBinaryModel* CBinaryModel::Create()
{
	CBinaryModel* instance = new CBinaryModel();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CBinaryModel");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CBinaryModel::Clone(INIT_DESC* pArg)
{
	CBinaryModel* instance = new CBinaryModel(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CBinaryModel");
		Safe_Release(instance);
	}

	return instance;
}

void CBinaryModel::Free()
{
	__super::Free();
	//m_Importer.FreeScene();
	//m_pAIScene = nullptr;
}


static _float3 Make_SubMesh_ExplodeDir(int index)
{
	const float angle = index * 2.39996323f; // golden angle ´À³¦
	const float x = cosf(angle);
	const float z = sinf(angle);
	const float y = 0.35f + 0.15f * sinf(index * 1.713f);

	_float3 dir = { x, y, z };

	const float len = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
	if (len > 0.0001f)
	{
		dir.x /= len;
		dir.y /= len;
		dir.z /= len;
	}

	return dir;
}

void CBinaryModel::Test()
{
	auto* materialComponent = Get_Component<CMaterial>();
	if (!materialComponent)
		return;

	auto& materialInstances = materialComponent->Get_MaterialInstances();

	if (isTesting)
	{
		isTesting = false;
		time = 0.f;

		for (auto& materialInstance : materialInstances)
			materialInstance->Reset_Pass();

		m_subMeshScatterIndices.clear();
		m_subMeshLocalCenters.clear();
	}
	else
	{
		isTesting = true;
		time = 0.f;

		m_subMeshScatterIndices.resize(materialInstances.size());
		m_subMeshLocalCenters.resize(materialInstances.size());

		for (size_t subsetIndex = 0; subsetIndex < materialInstances.size(); ++subsetIndex)
		{
			m_upBias = Helper::Get_Random_Float(-0.2f, 0.2f);

			auto& materialInstance = materialInstances[subsetIndex];
			m_subMeshScatterIndices[subsetIndex] = static_cast<uint32_t>(subsetIndex);
			m_subMeshLocalCenters[subsetIndex] = Get_Component<CStaticModel>()->Get_MeshBoundingBox(subsetIndex).GetHalfPoint();
			materialInstance->Set_Param("g_Time", { &time, "float", sizeof(float) });
			materialInstance->Set_Param("g_ScatterDistance", { &m_scatterDistance, "float", sizeof(float) });
			materialInstance->Set_Param("g_RotationStrength", { &m_rotationStrength, "float", sizeof(float) });
			materialInstance->Set_Param("g_UpBias", { &m_upBias, "float", sizeof(float) });

			materialInstance->Set_Param("SubMeshScatterIndex", { &m_subMeshScatterIndices[subsetIndex], "uint", sizeof(uint32_t) });
			materialInstance->Set_Param("SubMeshLocalCenter", { &m_subMeshLocalCenters[subsetIndex], "float3", sizeof(_float3) });

			materialInstance->Override_Pass("testOpaque");
		}
	}
}