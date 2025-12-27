#pragma once

#include "BasePanel.h"
#include "CamObject.h"

NS_BEGIN(Client)

class CCamPanel final : public CBasePanel
{
private:
    CCamPanel(GUI_CONTEXT* context) : CBasePanel(context) {}
    ~CCamPanel() = default;

public:
    virtual void Update_Panel(_float dt) override;
    virtual void Render_GUI() override;

public:
    void SetLevelTag(const string& levelTag) { m_levelTag = levelTag; m_needRefresh = true; }
    void RefreshCandidates();

private:
    void DrawMainCamSelector();

private:
    struct CamCandidate
    {
        OBJECT_HANDLE handle{};
        string        displayName{};
        string        uniqueId{};
    };

private:
    string               m_levelTag = "Test_Level";
    string               m_layerTag = "Camera_Layer";

    vector<CamCandidate> m_candidates{};
    int                  m_selectedIndex = 0;
    _bool                m_needRefresh = true;

public:
    static CCamPanel* Create(GUI_CONTEXT* context) { return new CCamPanel(context); }
    virtual void Free() override { __super::Free(); }
};

NS_END