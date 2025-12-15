#pragma once

#include "BasePanel.h"
#include "CamPanelData.h"

NS_BEGIN(CameraTool)

class CCamPanel final : public CBasePanel
{
private:
	CCamPanel(GUI_CONTEXT* context) : CBasePanel(context) {}
	~CCamPanel() = default;

	void Init();

public:
	void Update_Panel(_float dt) override;
	void Render_GUI() override;
	void SetCaptureTarget(CCamObj* camObj);

private:
	void  DrawToolbar();
	void  DrawCamSelector();
	void  DrawKeyframeArea();
	void  DrawKeyframeList();
	void  DrawKeyframeEditor();
	void  DrawTimeline();
	void  DrawInterpSelector();
	_bool DrawConstraintBar();
	_bool DrawOrbitTargetBar();

private: // Helper
    void    AddKey_Default();
    void    DeleteSelectedKey();
    void    SortKeysByTime_Stable();
    void    MergeNearDuplicateTimes_KeepLast();
    void    SyncEditorFromSelection();
    void    ApplyEditorToSelectedKey_TimeOnly();
    void    CaptureSelectedKey_FromCaptureCam();
	_bool   SelectKeyById(_uint keyId);
    _bool   HasValidSelection()  const;
	_uint   GetSelectedKeyId()   const;
	_float  GetNextDefaultTime() const;

	CamKeyFrame&               GetSelectedKey();
	vector<CamKeyFrame>&       GetKeyFrames()         { return target.sequence->keyframes; }
	const vector<CamKeyFrame>& GetKeyFrames()   const { return target.sequence->keyframes; }

private:
	CamSequenceDesc  debugSequence{};
	CamToolTarget    target{};
	CamToolEditState state{};
	CamToolKeyPolicy policy{};

public:
	static CCamPanel* Create(GUI_CONTEXT* context);
	virtual void Free() override;
};

NS_END