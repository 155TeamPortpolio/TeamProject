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
#include "Texture.h"

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
