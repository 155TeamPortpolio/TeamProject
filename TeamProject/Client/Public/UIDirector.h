#pragma once

#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance; class CUI_Object; class IUI_Service;
NS_END

NS_BEGIN(Client)

enum class UICanvas { Logo, End };

class CUIDirector final : public CBase
{
	DECLARE_SINGLETON(CUIDirector);
private:
	CUIDirector() {}
	virtual ~CUIDirector() = default;

public:
	void Initialize(const string& levelKey);

public:
	void PushEvent(UIEventType type, void* arg = {});

private:
	void SetActive(UICanvas canvas, void* arg = {});
	void SetActive(initializer_list<UICanvas> canvases, void* arg = {});

	void SetDeactive(UICanvas canvas);
	void SetDeactive(initializer_list<UICanvas> canvases);

private:
	string                                m_levelKey;
	array<UI_HANDLE, ENUM(UICanvas::End)> m_canvasHandles{};

public:
	virtual void Free() override;
};

NS_END