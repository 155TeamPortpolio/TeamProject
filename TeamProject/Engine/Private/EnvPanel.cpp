#include "Engine_Defines.h"
#include "EnvPanel.h"
#include "GameInstance.h"
CEnvPanel::CEnvPanel(GUI_CONTEXT* context)
	:CBasePanel(context)
{
}

CEnvPanel::~CEnvPanel()
{
}

HRESULT CEnvPanel::Initialize()
{
    Check_Env();
    return S_OK;
}
void CEnvPanel::Update_Panel(_float dt)
{
	
}

void CEnvPanel::Render_GUI()
{
    // ---- 애니메이션 ----
    const _float deltaTime = ImGui::GetIO().DeltaTime;
    const _float speed = 10.0f;
    const _float targetTime = m_isOpen ? 1.0f : 0.0f;
    m_fOpentime = m_fOpentime + (targetTime - m_fOpentime) * (1.0f - expf(-speed * deltaTime));
    if (m_fOpentime < 0.001f) m_fOpentime = 0.0f;
    if (m_fOpentime > 0.999f) m_fOpentime = 1.0f;

    // ---- 레이아웃(우측 상단 고정) ----
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 workPos = viewport->WorkPos;
    const ImVec2 workSize = viewport->WorkSize;

    const float buttonHeight = 24.0f;
    const float envButtonWidth = 40.0f;            // "ENV" 가독성용(높이는 buttonHeight)
    const float panelWidthClosed = envButtonWidth +18.0f;
    const float panelWidthOpen = 340.0f;

    const float panelHeightClosed = buttonHeight + 20.0f;
    const float panelHeightOpen = 420.0f;

    const float panelWidth = panelWidthClosed + (panelWidthOpen - panelWidthClosed) * (float)m_fOpentime;
    const float panelHeight = panelHeightClosed + (panelHeightOpen - panelHeightClosed) * (float)m_fOpentime;

    const float marginRight = 12.0f;
    const float marginTop = 72.0f;

    const ImVec2 panelPos(
        workPos.x + workSize.x - panelWidth - marginRight,
        workPos.y + marginTop
    );
    const ImVec2 panelSize(panelWidth, panelHeight);

    ImGui::SetNextWindowPos(panelPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(panelSize, ImGuiCond_Always);

    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoSavedSettings;

    // ---- 스타일 ----
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_isOpen?12.0f : 0, 10.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);

    ImVec4 bg = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
    const float minBgAlpha = 0.02f;  // 닫힘 상태에서 박스 거의 안 보이게
    const float maxBgAlpha = 0.92f;
    bg.w = minBgAlpha + (maxBgAlpha - minBgAlpha) * (float)m_fOpentime;
    ImGui::PushStyleColor(ImGuiCol_WindowBg, bg);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.08f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.14f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.95f, 1.0f));

    ImGui::Begin("##EnvDesc", nullptr, windowFlags);

    {
        const float contentWidth = ImGui::GetContentRegionAvail().x;
        ImVec2 envBtnSize(envButtonWidth, buttonHeight);
        ImGui::SetCursorPosX((contentWidth - envBtnSize.x) * 0.5f);

        if (ImGui::Button("ENV", envBtnSize))
        {
            if (m_bInitialized)
            {
                m_isOpen = !m_isOpen;
            }
            else
            {
                Check_Env();
                if (m_bInitialized)
                    m_isOpen = !m_isOpen;
            }
        }
    }

    // ---- 열림 컨텐츠 ----
    if (m_fOpentime > 0.01f)
    {
        ImGui::Spacing();

        // 타이틀 중앙 + 페이드
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, (float)m_fOpentime);

        const char* title = "Environment";
        float titleWidth = ImGui::CalcTextSize(title).x;
        float availWidth = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX((availWidth - titleWidth) * 0.5f);
        ImGui::TextUnformatted(title);

        ImGui::PopStyleVar();

        ImGui::Separator();

        // 내용 페이드
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, (float)m_fOpentime);
        ImGui::TextUnformatted("Fog");
        Render_FogDesc();
        ImGui::PopStyleVar(); // Alpha(내용)
    }

    if (m_isOpen && m_fOpentime > 0.8f)
    {
        if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
            ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            m_isOpen = false;
        }
    }

    ImGui::End();

    ImGui::PopStyleColor(5); // WindowBg + Button(3) + Text
    ImGui::PopStyleVar(3);   // Rounding, Padding, FrameRounding
}


void CEnvPanel::Check_Env()
{
		_bool hasFog = RenderSystem()->Get_FogDesc(m_fogSnapShot);

		if (hasFog) {
			m_bInitialized = true;
		}
}

void CEnvPanel::Render_FogDesc()
{
    if (!ImGui::CollapsingHeader("Fog", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    ImGui::Indent(4.0f);

    bool localDirty = false;
    m_cachedeFog = m_fogSnapShot;
    EditFogDescUI(m_fogSnapShot, localDirty);

    // 값이 바뀌었으면 바로 Set 하거나, Apply 버튼에서 Set 하거나 선택
    if (localDirty)
        m_FogDirty = true;

     if (m_FogDirty && ImGui::Button("Apply Fog")) { RenderSystem()->Set_FogDesc(m_fogSnapShot); m_FogDirty=false; }
     ImGui::SameLine();
     if (m_FogDirty && ImGui::Button("Revert Fog")) { m_fogSnapShot = m_cachedeFog; m_FogDirty=false; }
    ImGui::Unindent(4.0f);
}

 _bool CEnvPanel::EditFogDescUI(FOG_DESC& fogDesc, bool& outDirty)
{
     _bool changed = false;
     _bool useFog = fogDesc.IsUse;
    if (ImGui::Checkbox("Use Fog", &useFog))
    {
        fogDesc.IsUse = useFog;
        changed = true;
    }

    ImGui::BeginDisabled(!fogDesc.IsUse);

    // Color
    _float color[4] = { fogDesc.fogColor.x, fogDesc.fogColor.y, fogDesc.fogColor.z, fogDesc.fogColor.w };
    if (ImGui::ColorEdit4("Fog Color", color,
        ImGuiColorEditFlags_Float |
        ImGuiColorEditFlags_DisplayRGB |
        ImGuiColorEditFlags_InputRGB))
    {
        fogDesc.fogColor = _float4(color[0], color[1], color[2], color[3]);
        changed = true;
    }


    _float density = fogDesc.fogDensity;
    if (ImGui::SliderFloat("Density", &density, 0.0f, 0.05f, "%.5f"))
    {
        fogDesc.fogDensity = density;
        changed = true;
    }

    _float startValue = fogDesc.fogStart;
    _float endValue = fogDesc.fogEnd;

    if (ImGui::DragFloat("Start", &startValue, 0.1f, 0.0f, 100000.0f, "%.2f"))
    {
        fogDesc.fogStart = startValue;
        changed = true;
    }
    if (ImGui::DragFloat("End", &endValue, 0.1f, 0.0f, 100000.0f, "%.2f"))
    {
        fogDesc.fogEnd = endValue;
        changed = true;
    }

    if (fogDesc.fogEnd > 0.0f && fogDesc.fogStart > fogDesc.fogEnd)
    {
        swap(fogDesc.fogStart, fogDesc.fogEnd);
        changed = true;
    }

    ImGui::EndDisabled();

    if (changed)
        outDirty = true;

    return changed;
}
CEnvPanel* CEnvPanel::Create(GUI_CONTEXT* context)
{
	CEnvPanel* instance = new CEnvPanel(context);
	if (FAILED(instance->Initialize()))
		Safe_Release(instance);

	return instance;
}

void CEnvPanel::Free()
{
	__super::Free();
}

