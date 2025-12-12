#pragma once

#include "BasePanel.h"

NS_BEGIN(CameraTool)

class CamPanel final : public CBasePanel
{
private:
	CamPanel(GUI_CONTEXT* context) : CBasePanel(context) {}
	~CamPanel() = default;

	void Init();

public:
	void Update_Panel(_float dt) override;
	void Render_GUI() override;

private:
	void DrawToolbar();
	void DrawCamSelector();
	void DrawKeyframeArea();
	void DrawKeyframeList();
	void DrawKeyframeEditor();
	void DrawTimeline();

private:
	_bool recording = false;
	_bool playing   = false;
	_bool loop      = false;

	_uint selectedCam    = 0;
	_int  selectedKeyIdx = -1;

	_float curTime = 0.f;
	_float endTime = 10.f;
	_float zoom    = 1.f;

public:
	static CamPanel* Create(GUI_CONTEXT* context);
	virtual void Free() override;
};

NS_END