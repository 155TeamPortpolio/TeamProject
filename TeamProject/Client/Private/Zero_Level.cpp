#include "pch.h"
#include "Zero_Level.h"
#include "GameInstance.h"
#include "Helper_Func.h"
#include "Stage.h"
#include "ZeroStage_Boss.h"
#include "BattleSystem.h"

// Camera
#include "Camera.h"
#include "FreeCam.h"
#include "CamDirector.h"
#include "OrbitCam.h"
#include "ShadowCam.h"
#include "SequenceCam.h"
#include "CamPanel.h"
#include "CamLoader.h"


CZero_Level::CZero_Level(const string& LevelKey)
	:CLevel(LevelKey),
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CZero_Level::Initialize()
{
	auto boss = CZeroStage_Boss::Create(this);
	m_StageContainer.emplace(StageType::Boss, boss);

	m_Context.eStageType = StageType::Boss;
	m_Context.pNowStage = boss;
	m_Context.pNowStage->Ready_Stage(m_Context);
	return S_OK;
}

HRESULT CZero_Level::Awake()
{
	return S_OK;
}

void CZero_Level::Update()
{
	m_Context.pNowStage->Update();
}

HRESULT CZero_Level::Render()
{
	return S_OK;
}

void CZero_Level::PreLoad_Level()
{
	/*여기에 Add ResourcePath 넣기*/
}

HRESULT CZero_Level::ChangeStage(StageType nextStageType, _int StageID)
{
	if (m_Context.eStageType == nextStageType && m_Context.pNowStage)
		return S_OK;

	if (m_Context.pNowStage)
		m_Context.pNowStage->Exit_Stage(m_Context);

	auto found = m_StageContainer.find(nextStageType);
	if (found == m_StageContainer.end())
		return E_FAIL;

	m_Context.eStageType = nextStageType;
	m_Context.StageID = StageID;
	m_Context.pNowStage = found->second;

	return m_Context.pNowStage->Enter_Stage(m_Context);
}

void CZero_Level::Ready_Camera()
{
	constexpr _float aspect = (_float)g_iWinSizeX / g_iWinSizeY;

	auto seqCam = Builder::Create_Object({ "Zero_Level", "Proto_GameObject_SequenceCam" })
		.Camera(aspect)
		.Position({ 0.f, 2.f, -5.f })
		.Build("SequenceCam");

	auto freeCam = Builder::Create_Object({ "Zero_Level", "Proto_GameObject_FreeCam" })
		.Camera(aspect)
		.Position({ 0.f, 2.f, -3.f })
		.Build("FreeCam");

	CCT_DESC desc;
	desc.eGroup = COLLISION_GROUP::CAMERA;
	desc.iCollisionMask = ENUM(COLLISION_GROUP::COMMON);

	auto orbitCam = Builder::Create_Object({ "Zero_Level", "Proto_GameObject_OrbitCam" })
		.Camera(aspect)
		.CharacterController(desc)
		.Build("OrbitCam");

	ObjectManager()->Add_Object(seqCam, { "Zero_Level", "Camera_Layer" });
	ObjectManager()->Add_Object(freeCam, { "Zero_Level", "Camera_Layer" });
	ObjectManager()->Add_Object(orbitCam, { "Zero_Level", "Camera_Layer" });

	m_pCamDirector->SetCam(CamType::Sequence, seqCam->Get_Handle());
	m_pCamDirector->SetCam(CamType::Free, freeCam->Get_Handle());
	m_pCamDirector->SetCam(CamType::Orbit, orbitCam->Get_Handle());

	m_pCamDirector->SetReturnCam(CamType::Orbit);

	const OBJECT_HANDLE curPlayer = CBattleSystem::GetInstance()->GetCurCharacterHandle();
	static_cast<COrbitCam*>(orbitCam)->SetTarget(curPlayer);

	CamLoader::Load();

	CameraManager()->Set_MainCam(orbitCam->Get_Component<CCamera>());
}

CZero_Level* CZero_Level::Create(const string& LevelKey)
{
	CZero_Level* instance = new CZero_Level(LevelKey);
	if (FAILED(instance->Initialize())) {
		MSG_BOX("Test level Create Failed");
		Safe_Release(instance);
	}

	return instance;
}

void CZero_Level::Free()
{
	m_Context.pNowStage = nullptr;

	for (auto& pair : m_StageContainer)
		Safe_Release(pair.second);
	m_StageContainer.clear();

	__super::Free();
	m_pGameInstance->DestroyInstance();
}
