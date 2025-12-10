#include "Demo_Defines.h"
#include "DemoLevel.h"

#include "GameInstance.h"
#include "IInputService.h"
#include "ILevelService.h"

CDemoLevel::CDemoLevel(const string& LevelKey)
    : CLevel{ LevelKey },
    m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CDemoLevel::Initialize()
{
    return S_OK;
}

HRESULT CDemoLevel::Awake()
{
    return S_OK;
}

void CDemoLevel::Update()
{
}

HRESULT CDemoLevel::Render()
{
    SetWindowText(g_hWnd, TEXT("데모 레벨입니다."));
    return S_OK;
}

void CDemoLevel::PreLoad_Level()
{
}

CDemoLevel* CDemoLevel::Create(const string& LevelKey)
{
    CDemoLevel* instance = new CDemoLevel(LevelKey);
    if (FAILED(instance->Initialize())) {
        MSG_BOX("Demo level Create Failed");
        Safe_Release(instance);
    }

    return instance;
}

void CDemoLevel::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
}
