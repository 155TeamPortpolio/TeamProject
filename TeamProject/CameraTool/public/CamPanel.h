#pragma once

#include "BasePanel.h"
#include "CamPanelData.h"

#include "CamPanelUtil.h"

#define CAM   CGameInstance::GetInstance()->Get_CameraMgr()
#define GAME  CGameInstance::GetInstance()
#define OBJ   CGameInstance::GetInstance()->Get_ObjectMgr()

NS_BEGIN(CameraTool)

class CCamPanel final : public CBasePanel
{
private:
	CCamPanel(GUI_CONTEXT* context) : CBasePanel(context) {}
	~CCamPanel() = default;

	void    Init();

public:
	void    Update_Panel(_float dt) override;
	void    Render_GUI() override;
	void    SetCaptureTarget(CCamObj* camObj);
    void    SetSpaceReference(OBJECT_HANDLE handle);
    void    SetSpaceRefCandidates(initializer_list<OBJECT_HANDLE> handles);

private:
	void    DrawToolbar();
	void    DrawCamSelector();
	void    DrawKeyframeList();
	void    DrawKeyframeEditor();
	void    DrawTimeline();
	void    DrawHelpPopup();
	void    DrawWindowHeader();
	_bool   DrawConstraintBar();
	_bool   DrawOrbitTargetBar();

private:
    void    SetRecording(_bool on);
    void    SetPlaying(_bool on);
    void    RecalcEndTimeFromKeys();
    void    ClampCurTime();
    void    PostEdit_SequenceChanged();

private: 
	void    AddKey_Default();
	void    DeleteSelectedKey();
	void    SortKeysByTime_Stable();

    void    MergeNearDuplicateTimes(_uint preferKeyId = 0);

	void    SyncEditorFromSelection();
	void    ApplyEditorToSelectedKey_TimeOnly();
	void    CaptureSelectedKey_FromCaptureCam();
	_bool   SelectKeyById(_uint keyId);
	_bool   HasValidSelection()  const;
	_uint   GetSelectedKeyId()   const;
	_float  GetNextDefaultTime() const;

	CamKeyFrame&               GetSelectedKey();
	vector<CamKeyFrame>&       GetKeyFrames()       { return target.sequence->keyframes; }
	const vector<CamKeyFrame>& GetKeyFrames() const { return target.sequence->keyframes; }

private:
    struct KeyframeListUIState
    {
        _uint pendingDeleteKeyId = 0;

        string lastFileError{};
        bool requestOpenFileErrorPopup = false;

        char prefabNameBuf[128] = "DebugSequence";
        const CamSequenceDesc* lastSeqPtr = nullptr;
        bool nameEditing = false;
    };

    struct KeyframeEditorUIState
    {
        _uint pendingTimeSelectedId = 0;
        float pendingTimeValue = 0.f;
        int   pendingOverwriteCount = 0;
        bool  requestOpenTimeCollisionPopup = false;
    };

private:
    void   DrawKeyframeList_TopBar(vector<CamKeyFrame>& keys, bool& ioChangedAny);
    void   DrawKeyframeList_HeaderArea(vector<CamKeyFrame>& keys, bool& ioChangedAny);
    void   DrawKeyframeList_Table(vector<CamKeyFrame>& keys, bool& ioChangedAny);

    bool   ValidateCamPath(const string& pickedPath, string& outError) const;
    string GetDefaultCamFileName() const;
    void   SyncNameBufFromSeq();
    void   EnsureNameBufSync();
    void   DoSaveSequence();
    void   DoLoadSequence();

private:
    void   DrawKeyframeEditor_SelectedKeyTable(bool& ioChangedAny);
    void   DrawKeyframeEditor_OrbitArc(bool& ioChangedOrbit);

private:
    CamSequenceDesc       debugSequence{};
    CamToolTarget         target{};
    CamToolEditState      state{};
    CamToolKeyPolicy      policy{};

    KeyframeListUIState   keyListUI{};
    KeyframeEditorUIState keyEditUI{};
    vector<OBJECT_HANDLE> spaceRefCandidates{};

public:
    static CCamPanel* Create(GUI_CONTEXT* context);
    virtual void Free() override;
};

NS_END