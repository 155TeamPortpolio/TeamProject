#pragma once

#include "BasePanel.h"
#include "CamPanelData.h"

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
	void SetCaptureTarget(CamObj* camObj);

private:
	void DrawToolbar();
	void DrawCamSelector();
	void DrawKeyframeArea();
	void DrawKeyframeList();
	void DrawKeyframeEditor();
	void DrawTimeline();

private: // Helper
	vector<CamKeyFrame>&       GetKeys();
	const vector<CamKeyFrame>& GetKeys() const;
	bool                       HasValidSelection() const;
	CamKeyFrame&               GetSelectedKey();
	_uint                      GetSelectedKeyId() const;
	_float                     GetNextDefaultTime() const;
	void                       AddKey_Default();
	void                       DeleteSelectedKey();
	void                       SortKeysByTime_Stable();
	void                       MergeNearDuplicateTimes_KeepLast();
	bool                       SelectKeyById(_uint keyId);
	void                       SyncEditorFromSelection();
	void                       ApplyEditorToSelectedKey_TimeOnly();
	void                       CaptureSelectedKey_FromCaptureCam();
	void                       ApplyPlaybackAtTime(_float t);

private:
	CamSequenceDesc  debugSequence{};
	CamToolTarget    target{};
	CamToolEditState state{};
	CamToolKeyPolicy policy{};

public:
	static CamPanel* Create(GUI_CONTEXT* context);
	virtual void Free() override;
};

NS_END