#pragma once
#include "BasePanel.h"

NS_BEGIN(Engine)
	class CVideoPlayer;
	class CMFVideoDecoderBackend;
NS_END

NS_BEGIN(Client)
class CVideoPanel :
    public CBasePanel
{
private:
	CVideoPanel(GUI_CONTEXT* context);
	virtual ~CVideoPanel() DEFAULT;
	HRESULT Initialize();

public:
	virtual void Render_GUI() override;
	virtual void Update_Panel(_float dt) override;

private:
	CVideoPlayer* m_pPlayer = { nullptr };
	_uint64 m_startTimeSec = 0.0;
	_uint m_PlayerID = {};
	CMFVideoDecoderBackend* decoder = { nullptr };
public:
	static CVideoPanel* Create(GUI_CONTEXT* context);
	virtual void Free() override;
};
NS_END
