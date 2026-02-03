#include "pch.h"
#include "StageRouter.h"
#include "Stage.h"
#include "ZeroStage_Boss.h"
#include "ZeroStage_Normal.h"
#include "ZeroStage_Elite.h"
#include "Helper_Func.h"

CStageRouter::CStageRouter()
	:CLevelObject()
{

}
void CStageRouter::Render_GUI()
{
    static const char* typeNames[] = { "Normal", "Elite", "Boss", "Shop", "Rest", "End" };

    ImGui::Begin("Stage Map");

    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    ImDrawList* draw = ImGui::GetWindowDrawList();

    auto add = [](const ImVec2& a, const ImVec2& b) -> ImVec2 { return ImVec2(a.x + b.x, a.y + b.y); };
    auto sub = [](const ImVec2& a, const ImVec2& b) -> ImVec2 { return ImVec2(a.x - b.x, a.y - b.y); };

    if (canvasSize.x < 10.f || canvasSize.y < 10.f)
    {
        ImGui::Text("Canvas too small.");
        ImGui::End();
        return;
    }

    constexpr float stepX = 140.f;
    constexpr float stepY = 70.f;
    constexpr float nodeRadius = 14.f;

    ImVec2 canvasEnd = add(canvasPos, canvasSize);
    draw->AddRectFilled(canvasPos, canvasEnd, IM_COL32(20, 20, 20, 255));
    draw->PushClipRect(canvasPos, canvasEnd, true);

    ImVec2 origin = add(canvasPos, ImVec2(80.f, canvasSize.y * 0.5f));

    ImGui::SetCursorScreenPos(add(canvasPos, ImVec2(8.f, 8.f)));
    ImGui::Text("nodes: %d", (int)m_stageNodes.size());

    // ---- depth 범위 ----
    int maxDepth = 0;
    for (int nodeIndex = 0; nodeIndex < (int)m_stageNodes.size(); ++nodeIndex)
        if (m_stageNodes[nodeIndex].depth > maxDepth)
            maxDepth = m_stageNodes[nodeIndex].depth;

    // ---- depth별 개수 ----
    std::vector<int> depthCount(maxDepth + 1, 0);
    for (int nodeIndex = 0; nodeIndex < (int)m_stageNodes.size(); ++nodeIndex)
    {
        int depth = m_stageNodes[nodeIndex].depth;
        if (depth >= 0 && depth <= maxDepth)
            depthCount[depth] += 1;
    }

    // ---- depth별 중앙정렬 베이스 ----
    std::vector<float> depthBaseY(maxDepth + 1, 0.f);
    for (int depth = 0; depth <= maxDepth; ++depth)
    {
        int count = depthCount[depth];
        float half = (count > 0) ? (0.5f * (count - 1)) : 0.f;
        depthBaseY[depth] = -half * stepY;
    }

    // ---- depth별 커서 ----
    std::vector<int> depthCursor(maxDepth + 1, 0);

    // ---- (1) 먼저 nodeYOffset 채우기 ----
    std::vector<float> nodeYOffset(m_stageNodes.size(), 0.f);

    for (int nodeIndex = 0; nodeIndex < (int)m_stageNodes.size(); ++nodeIndex)
    {
        int depth = m_stageNodes[nodeIndex].depth;
        if (depth < 0 || depth > maxDepth)
            continue;

        int order = depthCursor[depth]++;
        nodeYOffset[nodeIndex] = depthBaseY[depth] + order * stepY;
    }

    // ---- (2) 그 다음 부모를 자식 평균으로 보정 ----
    for (int nodeIndex = (int)m_stageNodes.size() - 1; nodeIndex >= 0; --nodeIndex)
    {
        const StageNode& node = m_stageNodes[nodeIndex];
        if (node.ChildrenIndex.empty())
            continue;

        float sumY = 0.f;
        int count = 0;

        for (int childIndex : node.ChildrenIndex)
        {
            if (childIndex < 0 || childIndex >= (int)nodeYOffset.size())
                continue;

            sumY += nodeYOffset[childIndex];
            ++count;
        }

        if (count > 0)
            nodeYOffset[nodeIndex] = sumY / (float)count;
    }

    auto getNodePos = [&](int nodeIndex) -> ImVec2
        {
            const StageNode& node = m_stageNodes[nodeIndex];
            float x = origin.x + node.depth * stepX;
            float y = origin.y + nodeYOffset[nodeIndex];
            return ImVec2(x, y);
        };

    /* 1) 선 */
    for (int nodeIndex = 0; nodeIndex < (int)m_stageNodes.size(); ++nodeIndex)
    {
        ImVec2 from = getNodePos(nodeIndex);

        const StageNode& node = m_stageNodes[nodeIndex];
        for (int childIndex : node.ChildrenIndex)
        {
            if (childIndex < 0 || childIndex >= (int)m_stageNodes.size())
                continue;

            ImVec2 to = getNodePos(childIndex);
            draw->AddLine(from, to, IM_COL32(120, 120, 120, 255), 2.f);
        }
    }

    /* 2) 노드 */
    for (int nodeIndex = 0; nodeIndex < (int)m_stageNodes.size(); ++nodeIndex)
    {
        const StageNode& node = m_stageNodes[nodeIndex];
        ImVec2 pos = getNodePos(nodeIndex);

        ImU32 color = IM_COL32(150, 150, 150, 255);
        switch (node.MyType)
        {
        case StageType::Normal: color = IM_COL32(160, 160, 160, 255); break;
        case StageType::Elite:  color = IM_COL32(255, 120, 120, 255); break;
        case StageType::Boss:   color = IM_COL32(255, 220, 80, 255);  break;
        default: break;
        }

        draw->AddCircleFilled(pos, nodeRadius, color);
        draw->AddCircle(pos, nodeRadius, IM_COL32(30, 30, 30, 255), 0, 2.f);

        int typeIndex = (int)node.MyType;
        const char* label =
            (typeIndex >= 0 && typeIndex < (int)IM_ARRAYSIZE(typeNames)) ? typeNames[typeIndex] : "Unknown";

        // ---- 글자 흰색 + 가운데 ----
        ImVec2 textSize = ImGui::CalcTextSize(label);
        ImVec2 textPos = sub(pos, ImVec2(textSize.x * 0.5f, textSize.y * 0.5f));
        draw->AddText(textPos, IM_COL32(255, 255, 255, 255), label);
    }

    draw->PopClipRect();
    ImGui::End();
}


StageType CStageRouter::Pop_StageType()
{
	if (m_visitQueue.empty())
		return StageType::End;

	int nodeIndex = m_visitQueue.front();
	m_visitQueue.pop();

	for (int child : m_stageNodes[nodeIndex].ChildrenIndex)
		m_visitQueue.push(child);

	return m_stageNodes[nodeIndex].MyType;
}

/*스테이지 노드 추가*/
_int CStageRouter::AddNode(StageType type, _int parentIndex, _int depth)
{
	StageNode node{};
	node.MyType = type;
	node.ParentIndex = parentIndex;
	node.depth = depth;
	node.visited = (depth == 0);

	m_stageNodes.push_back(move(node));
	return static_cast<_int>(m_stageNodes.size() - 1);
}

void CStageRouter::BuildGraph(_int MaxDepth)
{
	m_stageNodes.clear();
	m_currentNode = -1;

	m_maxDepth = MaxDepth;

	_int RootNode = AddNode(StageType::Normal, -1, 0);
	m_currentNode = RootNode;

	vector<_int> layer;
	layer.push_back(RootNode);

	for (size_t depth = 0; depth < m_maxDepth; depth++)
	{
		vector<_int> nextLayer;

		for (_int parentIndex : layer)
		{
			_int childCount = RollChildCount(depth - 1, m_maxDepth);

			for (int i = 0; i < childCount; ++i)
			{
				StageType type = RollType(depth, m_maxDepth);
				_int childIndex = AddNode(type, parentIndex, depth);
				m_stageNodes[parentIndex].ChildrenIndex.push_back(childIndex);
				nextLayer.push_back(childIndex);
			}
		}
	}
}

_int CStageRouter::RollChildCount(_int depth, _int maxDepth)
{
	if (depth <= 1) 
		return 3;
	if (depth < maxDepth - 1) 
		return Helper::Get_Random_Int(2, 3);
	return 2;
}

StageType CStageRouter::RollType(_int depth, _int maxDepth)
{
	if (depth == maxDepth)
		return StageType::Boss;

	_float r = Helper::Get_Random_Float(0,1);

	// 너무 자주 Elite 나오지 않게 대충 제한
	if (r < 0.30f) return StageType::Normal;
	//if (r < 0.80f) return StageType::Shop;
	if (r < 0.55f) return StageType::Elite;
	return StageType::Normal;
}

_bool CStageRouter::Choose(_int choiceIndex)
{
	if (m_currentNode < 0) return false;

	auto& cur = m_stageNodes[m_currentNode];
	if (choiceIndex < 0 || choiceIndex >= static_cast<int>(cur.ChildrenIndex.size()))
		return false;

	int nextNode = cur.ChildrenIndex[choiceIndex];
	m_currentNode = nextNode;

	m_stageNodes[m_currentNode].visited = true;
	for (int child : m_stageNodes[m_currentNode].ChildrenIndex)
		m_stageNodes[child].visited = true;

	return true;
}

_int CStageRouter::GetChoiceCount()
{
	if (m_currentNode < 0) return 0;
	return static_cast<int>(m_stageNodes[m_currentNode].ChildrenIndex.size());
}

StageType CStageRouter::GetChoiceType(_int choiceIndex)
{
	if (m_currentNode < 0) return StageType::End;
	const auto& children = m_stageNodes[m_currentNode].ChildrenIndex;
	if (choiceIndex < 0 || choiceIndex >= static_cast<int>(children.size()))
		return StageType::End;
	return m_stageNodes[children[choiceIndex]].MyType;
}

StageType CStageRouter::GetCurrentType()
{
	if (m_currentNode < 0) return StageType::End;
	return m_stageNodes[m_currentNode].MyType;
}

_int CStageRouter::CreateNode(StageType type, _int parentIndex)
{
	StageNode node{};
	node.MyType = type;
	node.ParentIndex = parentIndex;
	m_stageNodes.push_back(move(node));
	return static_cast<_int>(m_stageNodes.size() - 1);
}


CStageRouter* CStageRouter::Create()
{
	CStageRouter* pInstance = new CStageRouter;
	if (FAILED(pInstance->Initialize_Prototype())) {
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CStageRouter::Clone(INIT_DESC* pArg)
{
	return nullptr;
}

void CStageRouter::Free()
{
	__super::Free();
}