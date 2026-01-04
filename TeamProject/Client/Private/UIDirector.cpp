#include "pch.h"
#include "UIDirector.h"
#include "GameInstance.h"
#include "UI_Object.h"
#include "UILoader.h"

IMPLEMENT_SINGLETON(CUIDirector);

void CUIDirector::Initialize(const string& levelKey)
{
	m_levelKey = levelKey;
	UILoader::Load(m_levelKey);
}

void CUIDirector::SetActive(UICanvas canvas, void* arg)
{
    auto& handle = m_canvasHandles[ENUM(canvas)];
    if (!handle.isValid()) return;
    handle.Get()->UI_Active(arg);
}

void CUIDirector::SetActive(initializer_list<UICanvas> canvases, void* arg)
{
    for (auto canvas : canvases)
        SetActive(canvas, arg);
}

void CUIDirector::SetDeactive(UICanvas canvas)
{
    auto& handle = m_canvasHandles[ENUM(canvas)];
    if (!handle.isValid()) return;
    handle.Get()->UI_DeActive();
}

void CUIDirector::SetDeactive(initializer_list<UICanvas> canvases)
{
    for (auto canvas : canvases)
        SetDeactive(canvas);
}

void CUIDirector::PushEvent(UIEventType type, void* arg)
{
    switch (type)
    {
    case UIEventType::Enter_Monitor:
        break;
    case UIEventType::Exit_Monitor:
        break;
    }
}

void CUIDirector::Free()
{
	__super::Free();

    for (auto& handle : m_canvasHandles)
        handle.Reset();
}