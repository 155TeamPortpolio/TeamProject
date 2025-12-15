#include "Engine_Defines.h"
#include "GameInstance.h"

#include "TimeMgr.h"
#include "InputMgr.h"
#include "AudioDevice.h"
#include "LevelMgr.h"
#include "GraphicDevice.h"
#include "PrototypeMgr.h"
#include "ObjectMgr.h"
#include "ResourceMgr.h"
#include "GUISystem.h"
#include "RenderSystem.h"
#include "CameraMgr.h"
#include "UI_Manager.h"
#include "LightMgr.h"
#include "RaySystem.h"
#include "CollisionSystem.h"
#include "FontSystem.h"
#include "PhysicsSystem.h"
#include "EventSystem.h"
#include "Level.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

CGameInstance::~CGameInstance()
{
}

_bool CGameInstance::Init_Engine(const ENGINE_DESC& engine)
{
	m_ClientRect.right = engine.iWinSizeX;
	m_ClientRect.bottom = engine.iWinSizeY;
	m_pGraphicDevice = CGraphicDevice::Create(engine, &m_pDevice, &m_pDeviceContext);
	m_pTimeManager = CTimeMgr::Create();
	m_pInputDevice = CInputMgr::Create(engine.hWnd);
	m_pSoundDevice = CAudioDevice::Create();
	m_pLevelManager = CLevelMgr::Create();
	m_pPrototypeManager = CPrototypeMgr::Create();
	m_pObjectManager = CObjectMgr::Create();
	m_pResourceManager = CResourceMgr::Create(m_pDevice, m_pDeviceContext);
	m_pCameraManager = CCameraMgr::Create();
	m_pUIManager = CUI_Manager::Create();
	m_pLightService = CLightMgr::Create();
	m_pRaySystem = CRaySystem::Create();
	m_pRenderSystem = CRenderSystem::Create(m_pDevice, m_pDeviceContext);
	m_pPhysicsSystem = CPhysicsSystem::Create();
	m_pCollisionSystem = CCollisionSystem::Create(m_pDevice, m_pDeviceContext);
	m_pFontSystem = CFontSystem::Create(m_pDevice, m_pDeviceContext);
	m_pEventSystem = CEventSystem::Create();


#if defined _USING_GUI
	m_pGuiSystem = CGUISystem::Create(engine, m_pDevice, m_pDeviceContext);
#endif

	m_pTimeManager->Add_Timer(G_EngineTimerID);
	Notify_LevelSet();
	return TRUE;
}

void CGameInstance::Notify_LevelSet()
{
	m_pPrototypeManager->Sync_To_Level();
	m_pObjectManager->Sync_To_Level();
	m_pResourceManager->Sync_To_Level();
	m_pUIManager->Sync_To_Level();
}

void CGameInstance::Update_EngineTimer()
{
	m_pTimeManager->Update_Timer(G_EngineTimerID);
}

_float CGameInstance::Get_EngineDeltaTime()
{
	return m_pTimeManager->Get_DeltaTime(G_EngineTimerID);
}

void CGameInstance::Set_EngineTimeScale(_float fScale)
{
	m_pTimeManager->Set_TimeScale(G_EngineTimerID, fScale);
}

void CGameInstance::Clear_LevelResource(const string& levelKey)
{
	if (levelKey.empty()) return;

	m_pPrototypeManager->Clear(levelKey);
	m_pResourceManager->Clear_Resource(levelKey);
	m_pObjectManager->Clear(levelKey);
	m_pUIManager->Clear(levelKey);

#ifdef _USING_GUI
	m_pGuiSystem->Get_Context()->pSelectedObject = nullptr;
#endif // _USING_GUI

}


void CGameInstance::Update_Engine(_float dt)
{
	_float realDt = m_pTimeManager->Get_RawDeltaTime(G_EngineTimerID);

	m_totalFrameCount++;


	/*엔진 제어 업데이트 -> 동기화용*/
	m_pObjectManager->Pre_EngineUpdate(realDt);
	m_pUIManager->Pre_EngineUpdate(realDt);


	/*클라 제어 업데이트 -> 게임 로직*/
	m_pObjectManager->Priority_Update(dt);
	m_pUIManager->Priority_Update(realDt);
	m_pLevelManager->Update(dt);
	m_pCameraManager->Update(dt);
	m_pObjectManager->Update(dt);
	m_pUIManager->Update(realDt);
	m_pRaySystem->Update(dt);
	m_pSoundDevice->Update();

#ifdef USINGPHYSICS
	m_pPhysicsSystem->Update(dt);
#endif // USINPHYSICS

#if defined _USING_GUI
	m_pGuiSystem->Update(realDt);
#endif
	m_pCollisionSystem->Update(dt);
	m_pObjectManager->Late_Update(dt);
	m_pUIManager->Late_Update(realDt);
#ifdef USINGPHYSICS
	m_pPhysicsSystem->Late_Update(dt);
#endif // USINPHYSICS
	/*엔진 제어 업데이트 -> 렌더 패킷 제출용*/
	m_pInputDevice->Update();
	m_pObjectManager->Post_EngineUpdate(realDt);
	m_pUIManager->Post_EngineUpdate(realDt);
}

void CGameInstance::Release_Engine()
{
	/*Managers*/
	Safe_Release(m_pTimeManager);
	
	Safe_Release(m_pLevelManager);
	Safe_Release(m_pPrototypeManager);
	Safe_Release(m_pObjectManager);
	Safe_Release(m_pResourceManager);
	Safe_Release(m_pRenderSystem);
	Safe_Release(m_pCameraManager);
	Safe_Release(m_pGuiSystem);
	Safe_Release(m_pUIManager);
	Safe_Release(m_pLightService);
	Safe_Release(m_pGraphicDevice);
	Safe_Release(m_pInputDevice);
	Safe_Release(m_pSoundDevice);
	Safe_Release(m_pRaySystem);
	Safe_Release(m_pCollisionSystem);
	Safe_Release(m_pFontSystem);
	Safe_Release(m_pPhysicsSystem);
	Safe_Release(m_pEventSystem);

	DestroyInstance();
}


CLevel* CGameInstance::Get_CurrentLevel()
{
	if (!m_pLevelManager)
		return nullptr;

	return m_pLevelManager->Get_CurrentLevel();
}

_bool CGameInstance::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#if defined _USING_GUI
	if (m_pGuiSystem)
	{
		_bool MessageCapture=m_pGuiSystem->Set_ProcHandler(hWnd, message, wParam, lParam);
		if(MessageCapture)
			return true;	// GUI에서 메시지를 먹었으면 여기서 끝
	}
#endif


	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return true;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
			return true;
		}
		break;

	case WM_INPUT:
		if (m_pInputDevice)
				m_pInputDevice->Process_Input(lParam);
		return true;

	case WM_SIZE:
		GetClientRect(hWnd, &m_ClientRect);
		break;

	default:
		break;
	}

	return false;
}

HRESULT CGameInstance::Draw_Begin(_float4* pColor)
{
	m_pGraphicDevice->Clear_BackBuffer_View(pColor);
	m_pGraphicDevice->Clear_DepthStencil_View();
	return S_OK;
}

HRESULT CGameInstance::Draw()
{
	m_pRenderSystem->Render();
	m_pLevelManager->Render(m_pDeviceContext);
#if defined _DEBUG
	m_pCollisionSystem->Render_Debug();
#endif
#if defined _USING_GUI
	m_pGuiSystem->Render_GUI();
#endif
	return S_OK;
}

HRESULT CGameInstance::Draw_End()
{
	m_pGraphicDevice->Present();
	return S_OK;
}

void CGameInstance::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	__super::Free();
}
