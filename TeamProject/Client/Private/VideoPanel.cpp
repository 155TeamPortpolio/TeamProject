#include "pch.h"
#include "VideoPanel.h"
#include "TestDecoder.h"
#include "GameInstance.h"
#include "MFVideoDecoderBackend.h"
CVideoPanel::CVideoPanel(GUI_CONTEXT* context) 
	:CBasePanel(context)
{

}

HRESULT CVideoPanel::Initialize()
{
    decoder = CMFVideoDecoderBackend::Create();
    decoder->SetLoop(true);
    m_PlayerID = VideoService()->CreatePlayer(decoder);
	m_pPlayer=  VideoService()->GetPlayer(m_PlayerID);
	CVideoPlayer::VIDEO_PLAYER_DESC desc;
	desc.filePath = "../Bin/Resources/Video/test.mp4"; // 더미는 무시함
	desc.loop = true;

	m_pPlayer->Open(desc);
	m_pPlayer->Play();
	VideoService()->StartDecode(m_PlayerID);
	m_startTimeSec = ImGui::GetTime();
	return S_OK;
}

void CVideoPanel::Render_GUI()
{
    ImGui::Text("State: %d", (int)m_pPlayer->GetState());
    if (!m_pPlayer)
    {
        ImGui::Text("No Video Player");
        return;
    }

    ID3D11ShaderResourceView* srv = m_pPlayer->GetSRV();
    if (!srv)
    {
        ImGui::Text("Video loading...");
        return;
    }
    ImGui::Text("push=%llu present=%llu",
        (unsigned long long)m_pPlayer->m_pushCount.load(),
        (unsigned long long)m_pPlayer->m_presentCount.load());

    // ImGui는 SRV를 ImTextureID로 그대로 받음 (DX11)
    ImVec2 size(640, 360); // 원하는 크기
    ImGui::Image(
        (ImTextureID)srv,
        size,
        ImVec2(0, 0),
        ImVec2(1, 1)
    );
}

void CVideoPanel::Update_Panel(_float dt)
{
	
}	

CVideoPanel* CVideoPanel::Create(GUI_CONTEXT* context) 
{
    CVideoPanel* instance = new CVideoPanel(context);
    if (FAILED(instance->Initialize())) {
        Safe_Release(instance);
    }
	return instance;
}

void CVideoPanel::Free()
{
    if (m_PlayerID != 0)
    {
        //VideoService()->StopDecode(m_PlayerID);
        VideoService()->DestroyPlayer(m_PlayerID);
        m_PlayerID = 0;
        m_pPlayer = nullptr;
    }
    Safe_Release(decoder);
}