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

    auto add = [](const ImVec2& left, const ImVec2& right) -> ImVec2 { return ImVec2(left.x + right.x, left.y + right.y); };

    if (canvasSize.x < 10.f || canvasSize.y < 10.f)
    {
        ImGui::TextUnformatted("Canvas too small.");
        ImGui::End();
        return;
    }

    constexpr float stepX = 140.f;
    constexpr float stepY = 70.f;
    constexpr float nodeRadiusBase = 14.f;

    const ImVec2 canvasEnd = add(canvasPos, canvasSize);
    draw->AddRectFilled(canvasPos, canvasEnd, IM_COL32(20, 20, 20, 255));
    draw->PushClipRect(canvasPos, canvasEnd, true);

    const float pad = 40.f;
    const ImVec2 originBase = add(canvasPos, ImVec2(pad, pad));

    const int nodeCount = (int)m_stageNodes.size();

    ImGui::SetCursorScreenPos(add(canvasPos, ImVec2(8.f, 8.f)));
    ImGui::Text("nodes: %d", nodeCount);

    if (nodeCount <= 0)
    {
        draw->PopClipRect();
        ImGui::End();
        return;
    }

    // ---- depth 범위 ----
    int maxDepth = 0;
    for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
        if (m_stageNodes[nodeIndex].depth > maxDepth)
            maxDepth = m_stageNodes[nodeIndex].depth;

    // ---- depth별 개수 ----
    std::vector<int> depthCount(maxDepth + 1, 0);
    for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
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

    // ---- (1) nodeYOffset 채우기 ----
    std::vector<float> nodeYOffset(nodeCount, 0.f);

    for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
    {
        int depth = m_stageNodes[nodeIndex].depth;
        if (depth < 0 || depth > maxDepth)
            continue;

        int order = depthCursor[depth]++;
        nodeYOffset[nodeIndex] = depthBaseY[depth] + order * stepY;
    }

    // ---- (2) 부모를 자식 평균으로 보정 ----
    for (int nodeIndex = nodeCount - 1; nodeIndex >= 0; --nodeIndex)
    {
        const StageNode& node = m_stageNodes[nodeIndex];
        if (node.ChildrenIndex.empty())
            continue;

        float sumY = 0.f;
        int childCount = 0;
        for (int childIndex : node.ChildrenIndex)
        {
            if (childIndex < 0 || childIndex >= nodeCount)
                continue;
            sumY += nodeYOffset[childIndex];
            ++childCount;
        }

        if (childCount > 0)
            nodeYOffset[nodeIndex] = sumY / (float)childCount;
    }

    // ==========================
    //  바운딩 + 비균일 스케일(X/Y 따로)
    // ==========================
    float minX = 1e9f, maxX = -1e9f;
    float minY = 1e9f, maxY = -1e9f;

    for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
    {
        const StageNode& node = m_stageNodes[nodeIndex];
        float xRaw = node.depth * stepX;
        float yRaw = nodeYOffset[nodeIndex];

        if (xRaw < minX) minX = xRaw;
        if (xRaw > maxX) maxX = xRaw;
        if (yRaw < minY) minY = yRaw;
        if (yRaw > maxY) maxY = yRaw;
    }

    float spanX = maxX - minX;
    float spanY = maxY - minY;
    if (spanX < 1.f) spanX = 1.f;
    if (spanY < 1.f) spanY = 1.f;

    float availX = canvasSize.x - pad * 2.f;
    float availY = canvasSize.y - pad * 2.f;
    if (availX < 1.f) availX = 1.f;
    if (availY < 1.f) availY = 1.f;

    float scaleX = availX / spanX;
    float scaleY = availY / spanY;

    // "압축만": 확대는 막기
    if (scaleX > 1.f) scaleX = 1.f;
    if (scaleY > 1.f) scaleY = 1.f;

    // 노드 크기는 더 작은 축 기준으로(원형 유지)
    float nodeRadius = nodeRadiusBase * ((scaleX < scaleY) ? scaleX : scaleY);
    if (nodeRadius < 6.f) nodeRadius = 6.f;

    const ImVec2 base = originBase;

    auto getNodePos = [&](int nodeIndex) -> ImVec2
        {
            const StageNode& node = m_stageNodes[nodeIndex];
            float xLocal = (node.depth * stepX - minX) * scaleX;
            float yLocal = (nodeYOffset[nodeIndex] - minY) * scaleY;
            return ImVec2(base.x + xLocal, base.y + yLocal);
        };

    // ==========================
    //  입력(클릭)
    //  - 현재 노드의 자식만 선택 가능
    //  - opened(열림) && !cleared(이미 지나감)만 이동
    // ==========================
    m_guiHoveredNode = -1;

    ImVec2 mousePos = ImGui::GetIO().MousePos;
    bool leftClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    bool canvasHovered =
        ImGui::IsWindowHovered() &&
        mousePos.x >= canvasPos.x && mousePos.x <= canvasEnd.x &&
        mousePos.y >= canvasPos.y && mousePos.y <= canvasEnd.y;

    int clickedNodeIndex = -1;

    // ==========================
    //  1) 선
    // ==========================
    for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
    {
        ImVec2 from = getNodePos(nodeIndex);
        const StageNode& node = m_stageNodes[nodeIndex];

        for (int childIndex : node.ChildrenIndex)
        {
            if (childIndex < 0 || childIndex >= nodeCount)
                continue;

            ImVec2 to = getNodePos(childIndex);

            // 선 색도 상태에 따라 살짝 다르게
            ImU32 lineColor = IM_COL32(120, 120, 120, 255);

            // 현재 노드 -> 자식 라인 강조
            if (nodeIndex == m_currentNode)
                lineColor = IM_COL32(160, 160, 160, 255);

            draw->AddLine(from, to, lineColor, 2.f);
        }
    }

    // ==========================
    //  2) 노드(그리기 + 히트 테스트)
    // ==========================
    for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
    {
        StageNode& node = m_stageNodes[nodeIndex];
        ImVec2 pos = getNodePos(nodeIndex);

        // 히트 테스트(원)
        ImVec2 delta = ImVec2(mousePos.x - pos.x, mousePos.y - pos.y);
        float dist2 = delta.x * delta.x + delta.y * delta.y;
        bool hovered = canvasHovered && (dist2 <= nodeRadius * nodeRadius);

        if (hovered)
            m_guiHoveredNode = nodeIndex;

        if (hovered && leftClicked)
            clickedNodeIndex = nodeIndex;

        // 상태 판단
        bool isCurrent = (nodeIndex == m_currentNode);
        bool isOpened = node.opened;     // 현재 코드 기준: opened == 열림(선택 가능)
        bool isCleared = node.cleared;    // 이미 지나간 노드(회색)

        // 현재 노드의 몇 번째 자식인지(선택 가능 판단용)
        int choiceOrder = -1;
        bool isSelectableNow = false;

        if (m_currentNode >= 0)
        {
            const auto& children = m_stageNodes[m_currentNode].ChildrenIndex;
            for (int childOrder = 0; childOrder < (int)children.size(); ++childOrder)
            {
                if (children[childOrder] == nodeIndex)
                {
                    choiceOrder = childOrder;
                    break;
                }
            }

            isSelectableNow = (choiceOrder >= 0) && isOpened && !isCleared;
        }

        // 색 결정
        ImU32 fillColor = IM_COL32(150, 150, 150, 255);

        if (isCleared)
        {
            // 이미 지나간 노드: 회색
            fillColor = IM_COL32(90, 90, 90, 255);
        }
        else if (!isOpened)
        {
            // 잠김: 더 어두운 회색
            fillColor = IM_COL32(50, 50, 50, 255);
        }
        else
        {
            // 열림(선택 후보): 타입 색
            switch (node.MyType)
            {
            case StageType::Normal: fillColor = IM_COL32(160, 160, 160, 255); break;
            case StageType::Elite:  fillColor = IM_COL32(255, 120, 120, 255); break;
            case StageType::Boss:   fillColor = IM_COL32(255, 220, 80, 255);  break;
            default: break;
            }

            // 현재 선택 가능한 자식이면 강조
            if (isSelectableNow)
                fillColor = IM_COL32(110, 220, 170, 255);
        }

        // 테두리(현재/호버)
        ImU32 borderColor = IM_COL32(30, 30, 30, 255);
        float borderThickness = 2.f;

        if (isCurrent)
        {
            borderColor = IM_COL32(80, 200, 255, 255);
            borderThickness = 3.f;
        }
        else if (hovered)
        {
            borderColor = IM_COL32(220, 220, 220, 255);
        }

        draw->AddCircleFilled(pos, nodeRadius, fillColor);
        draw->AddCircle(pos, nodeRadius, borderColor, 0, borderThickness);

        // 라벨
        int typeIndex = (int)node.MyType;
        const char* label =
            (typeIndex >= 0 && typeIndex < (int)IM_ARRAYSIZE(typeNames)) ? typeNames[typeIndex] : "Unknown";

        ImVec2 textSize = ImGui::CalcTextSize(label);
        ImVec2 textPos = ImVec2(pos.x - textSize.x * 0.5f, pos.y - textSize.y * 0.5f);
        draw->AddText(textPos, IM_COL32(255, 255, 255, 255), label);
    }

    // ==========================
    //  클릭 결과 처리
    //  - 현재 노드의 자식만 Choose로 이동
    // ==========================
    if (clickedNodeIndex >= 0 && clickedNodeIndex < nodeCount && m_currentNode >= 0)
    {
        const auto& children = m_stageNodes[m_currentNode].ChildrenIndex;

        for (int choiceOrder = 0; choiceOrder < (int)children.size(); ++choiceOrder)
        {
            if (children[choiceOrder] != clickedNodeIndex)
                continue;

            StageNode& clickedNode = m_stageNodes[clickedNodeIndex];

            // opened: 열림, cleared: 이미 지나감
            if (clickedNode.opened && !clickedNode.cleared)
            {
                dynamic_cast<CZero_Level*>(m_pOwner)->ChangeStage(clickedNode.MyType);
                Choose(choiceOrder);
                m_guiSelectedNode = clickedNodeIndex;
            }
            break;
        }
    }

    draw->PopClipRect();

    // 선택 정보(원하면)
    if (m_guiSelectedNode >= 0 && m_guiSelectedNode < nodeCount)
    {
        const StageNode& node = m_stageNodes[m_guiSelectedNode];
        ImGui::Separator();
        ImGui::Text("Selected: %d  Type: %s  opened: %s  cleared: %s",
            m_guiSelectedNode,
            typeNames[(int)node.MyType],
            node.opened ? "true" : "false",
            node.cleared ? "true" : "false");
    }

    ImGui::End();
}


/*스테이지 노드 추가*/
_int CStageRouter::AddNode(StageType type, _int parentIndex, _int depth)
{
	StageNode node{};
	node.MyType = type;
	node.ParentIndex = parentIndex;
	node.depth = depth;
    node.opened = (depth == 0); 
    node.cleared = (depth == 0); 

	m_stageNodes.push_back(move(node));
	return static_cast<_int>(m_stageNodes.size() - 1);
}

void CStageRouter::BuildGraph(_int MaxDepth, StageType root)
{
    m_stageNodes.clear();
    m_currentNode = -1;
    m_maxDepth = MaxDepth;

    _int rootNode = AddNode(root, -1, 0);
    m_currentNode = rootNode;

    vector<_int> layer;
    layer.push_back(rootNode);

    for (_int depth = 0; depth < m_maxDepth; ++depth)
    {
        vector<_int> nextLayer;
        nextLayer.reserve(layer.size() * 3);

        for (_int parentIndex : layer)
        {
            _int childCount = RollChildCount(depth, m_maxDepth);

            for (_int i = 0; i < childCount; ++i)
            {
                StageType type = RollType(depth + 1, m_maxDepth);
                _int childIndex = AddNode(type, parentIndex, depth + 1);

                m_stageNodes[parentIndex].ChildrenIndex.push_back(childIndex);
                nextLayer.push_back(childIndex);
            }
        }

        layer = move(nextLayer);
        if (layer.empty())
            break;
    }

    m_stageNodes[rootNode].opened = true; 
    m_stageNodes[rootNode].cleared = true; // 지나감 처리
    for (int child : m_stageNodes[rootNode].ChildrenIndex)
        m_stageNodes[child].opened = true; 
}


_int CStageRouter::GetChoiceNodeIndex(_int choiceIndex)
{
   if (m_currentNode < 0) return -1;
   const auto& children = m_stageNodes[m_currentNode].ChildrenIndex;
   if (choiceIndex < 0 || choiceIndex >= (int)children.size()) return -1;
   return children[choiceIndex];
}

_int CStageRouter::RollChildCount(_int depth, _int maxDepth)
{
	if (depth <= 1) 
		return 3;
	if (depth < maxDepth - 1) 
		return Helper::Get_Random_Int(1, 3);
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
    if (choiceIndex < 0 || choiceIndex >= (int)cur.ChildrenIndex.size())
        return false;

    int nextNode = cur.ChildrenIndex[choiceIndex];
    m_currentNode = nextNode;

    m_stageNodes[m_currentNode].cleared = true; // 지나감

    for (int child : m_stageNodes[m_currentNode].ChildrenIndex)
        m_stageNodes[child].opened = true;

    return true;
}
_int CStageRouter::GetChoiceCount()
{
	if (m_currentNode < 0) return 0;
	return static_cast<_int>(m_stageNodes[m_currentNode].ChildrenIndex.size());
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