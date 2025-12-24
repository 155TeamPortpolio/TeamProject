#include "pch.h"
#include "SequenceCam.h"
#include "GameInstance.h"

HRESULT CSequenceCam::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    m_seqPlayer = Add_Component<CCamSequencePlayer>();
    return S_OK;
}

HRESULT CSequenceCam::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
    m_seqPlayer = Get_Component<CCamSequencePlayer>();
    return S_OK;
}

CSequenceCam* CSequenceCam::Create()
{
    auto inst = new CSequenceCam();
    if (FAILED(inst->Initialize_Prototype()))
    {
        MSG_BOX("Object Create Failed : CSequenceCam");
        Safe_Release(inst);
    }
    return inst;
}

CGameObject* CSequenceCam::Clone(INIT_DESC* pArg)
{
    auto inst = new CSequenceCam(*this);
    if (FAILED(inst->Initialize(pArg)))
    {
        MSG_BOX("Object Clone Failed : CSequenceCam");
        Safe_Release(inst);
    }
    return inst;
}

void CSequenceCam::Render_GUI()
{
    __super::Render_GUI();

    if (ImGui::CollapsingHeader(u8"SequenceCam", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("SequenceCam_RenderGUI");

        auto myCam = Get_Component<CCamera>();
        bool isMain = (CAM->Get_BaseCam() == myCam);

        if (ImGui::Checkbox(u8"MainCam", &isMain))
        {
            if (isMain)
                CAM->Set_MainCam(myCam);
        }

        const _bool playing = IsPlaying();

        ImGui::Text("Playing: %s", playing ? "true" : "false");
        ImGui::Text("Last: %s", m_LastPath.string().c_str());

        if (m_seqPlayer)
        {
            const float t = m_seqPlayer->GetTime();
            const float s = m_seqPlayer->GetTimeScale();
            ImGui::Text("Time: %.3f", t);
            ImGui::Text("TimeScale: %.3f", s);
        }

        if (ImGui::Button(u8"Stop", ImVec2(160.f, 0.f)))
            Stop(true);

        ImGui::PopID();
    }
}