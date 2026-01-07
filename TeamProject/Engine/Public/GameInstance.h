  #pragma once
#include "Base.h"
#include "Engine_Service.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameInstance :
	public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	explicit CGameInstance();
	virtual ~CGameInstance();

public:
	_bool Init_Engine(const ENGINE_DESC& engine);
	void Update_Engine(_float dt);
	void Release_Engine();
	class CLevel* Get_CurrentLevel();
public:
	void Notify_LevelSet();
	void Clear_LevelResource(const string& levelKey);
	_bool HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT Draw_Begin(_float4* pColor);
	HRESULT Draw();
	HRESULT Draw_End();

public:
	void Update_EngineTimer();
	_float Get_EngineDeltaTime();
	void Set_EngineTimeScale(_float fScale);

public:
	ID3D11Device* Get_Device() { return m_pDevice; };
	ID3D11DeviceContext* Get_Context() { return m_pDeviceContext; };
	_float2 Get_ClientSize() { return _float2(m_ClientRect.right, m_ClientRect.bottom); }
	_uint Get_FrameCount() const { return m_totalFrameCount; }

#pragma region Game_Managers
public:
	class IGraphicService* Get_GraphicDev() { return m_pGraphicDevice; }
	class ITimeService* Get_TimeMgr() { return m_pTimeManager; }
	class IInputService* Get_InputDev() { return m_pInputDevice; }
	class IAudioService* Get_AudioDev() { return m_pSoundDevice; }
	class ILevelService* Get_LevelMgr() { return m_pLevelManager; }
	class IProtoService* Get_PrototypeMgr() { return m_pPrototypeManager; }
	class IObjectService* Get_ObjectMgr() { return m_pObjectManager; }
	class IResourceService* Get_ResourceMgr() { return m_pResourceManager; }
	class IGUIService* Get_GUISystem() { return m_pGuiSystem; }
	class IRenderService* Get_RenderSystem() { return m_pRenderSystem; }
	class ICameraService* Get_CameraMgr() { return m_pCameraManager; }
	class IUI_Service* Get_UIMgr() { return m_pUIManager; }
	class ILightService* Get_LightMgr() { return m_pLightService; }
	class IRayService* Get_RayMgr() { return m_pRaySystem; }
	class ICollisionService* Get_CollisionSystem () { return m_pCollisionSystem; }
	class IFontService* Get_FontSystem () { return m_pFontSystem; }
	class IPhysicsService* Get_PhysicsSystem() { return m_pPhysicsSystem; }
	class CEventSystem* Get_EventSystem() { return m_pEventSystem; }
	class CClickManager* Get_ClickMgr() { return m_pClickManager; }

private:
	class IGraphicService* m_pGraphicDevice = { nullptr };
	class ITimeService* m_pTimeManager = { nullptr };
	class IInputService* m_pInputDevice = { nullptr };
	class IAudioService* m_pSoundDevice = { nullptr };
	class ILevelService* m_pLevelManager = { nullptr };
	class IProtoService* m_pPrototypeManager = { nullptr };
	class IObjectService* m_pObjectManager = { nullptr };
	class IResourceService* m_pResourceManager = { nullptr };
	class IGUIService* m_pGuiSystem = { nullptr };
	class IRenderService* m_pRenderSystem = { nullptr };
	class ICameraService* m_pCameraManager = { nullptr };
	class IUI_Service* m_pUIManager = { nullptr };
	class ILightService* m_pLightService = { nullptr };
	class IRayService* m_pRaySystem = { nullptr };
	class ICollisionService* m_pCollisionSystem = { nullptr };
	class IFontService* m_pFontSystem = { nullptr };
	class IPhysicsService* m_pPhysicsSystem = { nullptr };
	class CEventSystem* m_pEventSystem = { nullptr };
	class CClickManager* m_pClickManager = { nullptr };
#pragma endregion

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	RECT m_ClientRect = {};
	_uint m_totalFrameCount = {};
public:
	virtual void Free() override;
};

inline auto* GraphicDevice() { return CGameInstance::GetInstance()->Get_GraphicDev(); }
inline auto* InputDevice() { return CGameInstance::GetInstance()->Get_InputDev(); }
inline auto* AudioDevice() { return CGameInstance::GetInstance()->Get_AudioDev(); }

inline auto* TimeManger() { return CGameInstance::GetInstance()->Get_TimeMgr(); }
inline auto* LevelManger() { return CGameInstance::GetInstance()->Get_LevelMgr(); }
inline auto* PrototypeManger() { return CGameInstance::GetInstance()->Get_PrototypeMgr(); }
inline auto* ObjectManger() { return CGameInstance::GetInstance()->Get_ObjectMgr(); }
inline auto* UIManager() { return CGameInstance::GetInstance()->Get_UIMgr(); }
inline auto* ResourceManger() { return CGameInstance::GetInstance()->Get_ResourceMgr(); }
inline auto* CameraManager() { return CGameInstance::GetInstance()->Get_CameraMgr(); }
inline auto* LightManager() { return CGameInstance::GetInstance()->Get_LightMgr(); }
inline auto* ClickManager() { return CGameInstance::GetInstance()->Get_ClickMgr(); }

inline auto* GUISystem() { return CGameInstance::GetInstance()->Get_GUISystem(); }
inline auto* RenderSystem() { return CGameInstance::GetInstance()->Get_RenderSystem(); }
inline auto* RaySystem() { return CGameInstance::GetInstance()->Get_RayMgr(); }
inline auto* CollisionSystem() { return CGameInstance::GetInstance()->Get_CollisionSystem(); }
inline auto* PhysicsSystem() { return CGameInstance::GetInstance()->Get_PhysicsSystem(); }
inline auto* FontSystem() { return CGameInstance::GetInstance()->Get_FontSystem(); }
inline auto* EventSystem() { return CGameInstance::GetInstance()->Get_EventSystem(); }

NS_END