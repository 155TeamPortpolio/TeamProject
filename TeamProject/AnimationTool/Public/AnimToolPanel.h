#pragma once
#include "AnimationTool_Defines.h"
#include "BasePanel.h"
#include "Assimps.h"

class CAnimToolPanel
	: CBasePanel
{
private:
    CAnimToolPanel(GUI_CONTEXT* pContext);
    virtual ~CAnimToolPanel() DEFAULT;

public:
    virtual void Update_Panel(_float dt) override;
    virtual void Render_GUI() override;

private:
    void Extract_Clip();


private:
    const aiScene*      m_pAIScene = { nullptr };
    Assimp::Importer	m_Importer = {};

public:
    static CBasePanel* Create(GUI_CONTEXT* context);
    virtual void Free() override;
};

