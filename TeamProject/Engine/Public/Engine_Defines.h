#ifndef Engine_Define_h__
#define Engine_Define_h__
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#include <Effect_Inc/d3dx11effect.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>

#include "DirectXTK_Inc/WICTextureLoader.h"
#include "DirectXTK_Inc/DDSTextureLoader.h"
#include "DirectXTK_Inc/ScreenGrab.h"
#include "DirectXTK_Inc/SpriteFont.h"
#include "DirectXTK_Inc/SpriteBatch.h"
#include "DirectXTK_Inc/VertexTypes.h"
#include "DirectXTK_Inc/PrimitiveBatch.h"
#include "DirectXTK_Inc/Effects.h"
#include <DirectXTK_Inc/CommonStates.h>
#include <DirectXTK_Inc/SimpleMath.h>
#include "ThirdPartyCsv.h"

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <ctime>
#include <memory>
#include <process.h>
#include <typeindex>
#include <filesystem>
#include <sstream>
#include <any>
#include <iostream>
#include <fstream>
#include <variant>
#include <stack>
#include <atomic>                           // m_stop, m_active 같은 원자 변수
#include <condition_variable>     // 작업 대기/깨우기
#include <deque>                           // 작업 큐 (앞에서 pop)
#include <future>                           // std::future / std::packaged_task
#include <mutex>                            // 뮤텍스
#include <thread>                           // 스레드 생성/join
#include <type_traits>                  // std::invoke_result_t

#include <commdlg.h>										// GetSaveFileName API를 위해 필요
#pragma comment(lib, "Comdlg32.lib")	// 라이브러리 링크

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

#include <FMOD_Inc/fmod.hpp>

// 기존 new 매크로로 인해 충돌5 방지
#pragma push_macro("new")
#undef new
#include "GUI_Inc/imgui.h"
#include "GUI_Inc/ImGuizmo.h"
#include "GUI_Inc/backends/imgui_impl_win32.h"
#include "GUI_Inc/backends/imgui_impl_dx11.h"
#include "GUI_Inc/imgui_stdlib.h"
#include "NFD_Inc/nfd.h"
#pragma pop_macro("new")

//---JSON -- //
#include "Json_Inc/json.hpp"  
using json = nlohmann::json;

//--PhysX--//
#pragma push_macro("new") 
#undef new 
#include "PhysX_Inc/PxPhysicsAPI.h"
#define USINGPHYSICS
//#define USE_MULTITHREAD_PHYSICS
using namespace physx;
#pragma pop_macro("new")

#define UNICODE
#define _UNICODE

#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"
#include "Engine_RenderStruct.h"
#include "Engine_Layouts.h"
#include "Build_Struct.h"
#include "Data_Packets.h"
#include "PreLoad_Struct.h"
#include "AnimationLayout.h"
#include "Humanoid.h"

using namespace Engine;

#include <windowsx.h>

#pragma warning(disable :4251) //맵 컨테이너 경고 무시

#define _USING_GUI


#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif

//--------------------Profile-------------------------
#ifdef _USING_GUI
#ifndef PROFILER_MAX_EVENTS
#define PROFILER_MAX_EVENTS 2048
#endif

#ifndef PROFILER_NAME_CAP
#define PROFILER_NAME_CAP 64
#endif

#ifndef PROFILER_HISTORY_FRAMES
#define PROFILER_HISTORY_FRAMES 240
#endif

struct ImgUiFrameProfiler
{
    struct EventRecord
    {
        char        name[PROFILER_NAME_CAP];
        int64_t     startTicks;
        int64_t     endTicks;
        int         depth;
    };

    struct StackItem
    {
        int eventIndex;
    };

    // timer
    LARGE_INTEGER qpcFrequency;
    int64_t       ticksPerSecond;

    // frame
    uint32_t      frameIndex;
    int64_t       frameStartTicks;
    int64_t       frameEndTicks;

    // events (current frame)
    EventRecord   events[PROFILER_MAX_EVENTS];
    int           eventCount;

    StackItem     stack[PROFILER_MAX_EVENTS];
    int           stackCount;

    // history: total ms only (ring)
    float         totalMsHistory[PROFILER_HISTORY_FRAMES];
    uint32_t      totalMsHistoryCursor;

    // ui options
    bool          enable;
    bool          showOnlyRoot;     // false면 전체 트리 출력
    bool          showGraph;

    void Initialize()
    {
        QueryPerformanceFrequency(&qpcFrequency);
        ticksPerSecond = (int64_t)qpcFrequency.QuadPart;

        frameIndex = 0;
        frameStartTicks = 0;
        frameEndTicks = 0;

        eventCount = 0;
        stackCount = 0;

        memset(totalMsHistory, 0, sizeof(totalMsHistory));
        totalMsHistoryCursor = 0;

        enable = true;
        showOnlyRoot = false;
        showGraph = true;
    }

    static int64_t NowTicks()
    {
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        return (int64_t)counter.QuadPart;
    }

    float TicksToMs(int64_t ticks) const
    {
        // ms = ticks * 1000 / freq
        return (float)((double)ticks * 1000.0 / (double)ticksPerSecond);
    }

    void BeginFrame(uint32_t newFrameIndex)
    {
        frameIndex = newFrameIndex;
        eventCount = 0;
        stackCount = 0;
        frameStartTicks = NowTicks();
        frameEndTicks = frameStartTicks;
    }

    void EndFrame()
    {
        frameEndTicks = NowTicks();

        // 혹시 end가 안 찍힌 이벤트들 마감
        for (int eventIndex = 0; eventIndex < eventCount; ++eventIndex)
        {
            if (events[eventIndex].endTicks == 0)
                events[eventIndex].endTicks = frameEndTicks;
        }

        float totalMs = TicksToMs(frameEndTicks - frameStartTicks);
        totalMsHistory[totalMsHistoryCursor % PROFILER_HISTORY_FRAMES] = totalMs;
        totalMsHistoryCursor++;
    }

    // 수동 시작/끝 (원하면 직접 호출)
    int BeginEvent(const char* eventName)
    {
        if (!enable) return -1;
        if (!eventName) eventName = "(null)";
        if (eventCount >= PROFILER_MAX_EVENTS) return -1;
        if (stackCount >= PROFILER_MAX_EVENTS) return -1; // <- 추가 (중요)

        const int eventIndex = eventCount++;
        EventRecord& record = events[eventIndex];

        record.name[0] = '\0';
        strncpy_s(record.name, eventName, PROFILER_NAME_CAP - 1);

        record.startTicks = NowTicks();
        record.endTicks = 0;
        record.depth = stackCount;

        stack[stackCount].eventIndex = eventIndex;
        stackCount++;
        return eventIndex;
    }


    void EndEvent()
    {
        if (!enable) return;
        if (stackCount <= 0) return;

        stackCount--;
        const int eventIndex = stack[stackCount].eventIndex;
        if (eventIndex >= 0 && eventIndex < eventCount)
        {
            events[eventIndex].endTicks = NowTicks();
        }
    }

    // RAII scope helper
    struct Scope
    {
        ImgUiFrameProfiler* profiler;
        bool active;
        Scope(ImgUiFrameProfiler* ownerProfiler, const char* eventName)
            : profiler(ownerProfiler), active(false)
        {
            if (profiler && profiler->enable)
            {
                profiler->BeginEvent(eventName);
                active = true;
            }
        }
        ~Scope()
        {
            if (profiler && active)
                profiler->EndEvent();
        }
    };

    // UI
    void RenderImGui(const char* windowTitle = "Frame Profiler")
    {
        if (!enable) return;

        if (!ImGui::Begin(windowTitle))
        {
            ImGui::End();
            return;
        }

        const float frameMs = TicksToMs(frameEndTicks - frameStartTicks);

        ImGui::Text("frame=%u  total=%.3f ms  events=%d", (unsigned)frameIndex, frameMs, eventCount);
        ImGui::Checkbox("Show graph", &showGraph);
        ImGui::SameLine();
        ImGui::Checkbox("Only root", &showOnlyRoot);

        if (showGraph)
        {
            // 최근 값부터 보이게 임시 배열 재정렬
            float temp[PROFILER_HISTORY_FRAMES];
            const uint32_t cursor = totalMsHistoryCursor;
            for (uint32_t offsetIndex = 0; offsetIndex < PROFILER_HISTORY_FRAMES; ++offsetIndex)
            {
                uint32_t srcIndex = (cursor + offsetIndex) % PROFILER_HISTORY_FRAMES;
                temp[offsetIndex] = totalMsHistory[srcIndex];
            }
            ImGui::PlotLines("Total ms", temp, PROFILER_HISTORY_FRAMES, 0, nullptr, 0.0f, 0.0f, ImVec2(0, 70));
        }

        ImGui::Separator();

        if (ImGui::BeginTable("ProfilerTable", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
        {
            ImGui::TableSetupColumn("Event");
            ImGui::TableSetupColumn("ms", ImGuiTableColumnFlags_WidthFixed, 110.0f);
            ImGui::TableSetupColumn("%", ImGuiTableColumnFlags_WidthFixed, 70.0f);
            ImGui::TableHeadersRow();

            for (int eventIndex = 0; eventIndex < eventCount; ++eventIndex)
            {
                const EventRecord& record = events[eventIndex];

                // root만 보이게
                if (showOnlyRoot && record.depth != 0)
                    continue;

                const int64_t endTicksSafe = (record.endTicks != 0) ? record.endTicks : frameEndTicks;
                const float eventMs = TicksToMs(endTicksSafe - record.startTicks);
                const float percent = (frameMs > 0.0f) ? (eventMs / frameMs * 100.0f) : 0.0f;

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                // indent depth
                if (record.depth > 0)
                    ImGui::Indent((float)record.depth * 12.0f);

                ImGui::TextUnformatted(record.name);

                if (record.depth > 0)
                    ImGui::Unindent((float)record.depth * 12.0f);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.3f", eventMs);

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.1f", percent);
            }

            ImGui::EndTable();
        }

        ImGui::End();
    }
};

// 매크로: 스코프 자동 측정
#define PROFILER_CONCAT_INNER(left, right) left##right
#define PROFILER_CONCAT(left, right) PROFILER_CONCAT_INNER(left, right)
#define PROFILE_SCOPE(profilerPtr, eventNameLiteral) \
    ImgUiFrameProfiler::Scope PROFILER_CONCAT(profilerScope_, __COUNTER__)(profilerPtr, eventNameLiteral)

extern ImgUiFrameProfiler g_Profiler;
#endif

#endif // Engine_Define_h__
