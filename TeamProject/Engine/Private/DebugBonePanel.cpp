#include "Engine_Defines.h"
#include "DebugBonePanel.h"
#include "Animator3D.h"
#include "GameObject.h"
#include "SkeletalModel.h"
#include "ModelData.h"
#include "GameInstance.h"

CDebugBonePanel::CDebugBonePanel(GUI_CONTEXT* context)
	:CBasePanel(context)
{
}

HRESULT CDebugBonePanel::Initialize()
{

	return S_OK;
}

void CDebugBonePanel::Render_GUI()
{
	ImGui::SeparatorText("Debug Skeleton");

	ImGui::Checkbox("Enable", &m_bEnable);
	ImGui::SameLine();
	ImGui::Checkbox("Names", &m_bDrawNames);
	ImGui::SameLine();
	ImGui::Checkbox("Joints", &m_bDrawJoints);

	ImGui::SliderFloat("Line Thickness", &m_fLineThickness, 1.0f, 4.0f, "%.1f");
	ImGui::SliderFloat("Joint Radius", &m_fJointRadius, 1.0f, 8.0f, "%.1f");

	CGameObject* pTarget = m_pContext->pSelectedObject;
	if (pTarget == nullptr)
	{
		ImGui::TextDisabled("No target selected.");
		return;
	}

	CSkeletalModel* pModel = pTarget->Get_Component<CSkeletalModel>();
	if (pModel == nullptr) {
		ImGui::TextDisabled("Target is Not Skinned");
		return;
	}

	CModelData* pData = pModel->Get_ModelData();
	if (pData == nullptr) {
		ImGui::TextDisabled("Empty Model Data");
		return;
	}

	const auto& bones = pData->Get_BoneNames();

	vector<uint8_t> isAncestor(bones.size(), 0);
	if (m_iSelectedBone >= 0 && m_iSelectedBone < (int)bones.size())
	{
		int p = pData->Get_BoneParentIndex(m_iSelectedBone);
		while (p != -1)
		{
			if (p < 0 || p >= (int)bones.size()) break;
			isAncestor[p] = 1;
			p = pData->Get_BoneParentIndex(p);
		}
	}

	ImGui::SeparatorText("Bones");
	ImGui::BeginChild("##BoneList", ImVec2(0, 220), true);

	for (int i = 0; i < (int)bones.size(); ++i)
	{
		const _bool selected = (m_iSelectedBone == i);

		ImVec4 textCol = ImGui::GetStyleColorVec4(ImGuiCol_Text);
		if (selected)              textCol = ImVec4(1.f, 0.35f, 0.35f, 1.f); // 선택: 빨강
		else if (isAncestor[i])    textCol = ImVec4(1.f, 0.9f, 0.25f, 1.f);  // 조상: 노랑

		ImGui::PushStyleColor(ImGuiCol_Text, textCol);

		if (ImGui::Selectable(bones[i].c_str(), selected))
			m_iSelectedBone = i;

		ImGui::PopStyleColor();

		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			int parent = pData->Get_BoneParentIndex(i);
			if (parent != -1)
			{
				ImGui::TextColored(ImVec4(0.7f, 0.9f, 1.f, 1.f), "parent : %s", bones[parent].c_str());
			}
			else
			{
				ImGui::TextDisabled("No parent");
			}
			ImGui::EndTooltip();
		}
	}

	ImGui::EndChild();
	ImGui::Text("Selected Bone: %d", m_iSelectedBone);
}

void CDebugBonePanel::Update_Panel(_float dt)
{
	if (!m_bEnable) return;

	CGameObject* pTarget = m_pContext->pSelectedObject;
	if (!pTarget) return;

	ImVec2 vpPos = ImGui::GetMainViewport()->Pos;
	ImVec2 vpSize = ImGui::GetMainViewport()->Size;

	DrawSkeletonOverlay_ImGui(pTarget, vpPos, vpSize);
}

_bool CDebugBonePanel::WorldToScreen(const _float3& world, const _float4x4& view, const _float4x4& proj, const ImVec2& vpPos, const ImVec2& vpSize, ImVec2& out) const
{
	Matrix VP = view * proj;

	Vector4 clip = Vector4::Transform(Vector4(world.x, world.y, world.z, 1.f), VP);

	if (clip.w <= 0.0001f) return false; // 카메라 뒤

	float ndcX = clip.x / clip.w;
	float ndcY = clip.y / clip.w;

	out.x = vpPos.x + (ndcX * 0.5f + 0.5f) * vpSize.x;
	out.y = vpPos.y + (-ndcY * 0.5f + 0.5f) * vpSize.y; // Y flip

	return true;
}

void CDebugBonePanel::DrawSkeletonOverlay_ImGui(CGameObject* target, const ImVec2& vpPos, const ImVec2& vpSize)
{
	if (!m_bEnable || !target) return;

	CSkeletalModel* pModel = target->Get_Component<CSkeletalModel>();
	CTransform* pTransform = target->Get_Component<CTransform>();
	CAnimator3D* pAnimator = target->Get_Component<CAnimator3D>();

	if (!pModel || !pTransform) return;
	_bool Animating = false;

	if (pAnimator)
		Animating = true;

	CModelData* pData = pModel->Get_ModelData();
	if (!pData) return;

	const auto& boneNames = pData->Get_BoneNames();
	const int boneCount = (int)boneNames.size();
	if (boneCount <= 0) return;

	auto CamMgr = CGameInstance::GetInstance()->Get_CameraMgr();

	Matrix view, proj;
	{
		view = *CamMgr->Get_ViewMatrix();
		proj = *CamMgr->Get_ProjMatrix();
	}

	vector<_float4x4> boneModel;

	if (Animating)
		boneModel = pAnimator->Get_BoneMatrices();
	else
		boneModel = pModel->Get_BoneMatrices();
	
	Matrix ownerWorld = pTransform->Get_WorldMatrix();

	vector<uint8_t> isAncestor(boneCount, 0);
	if (m_iSelectedBone >= 0 && m_iSelectedBone < boneCount)
	{
		int p = pData->Get_BoneParentIndex(m_iSelectedBone);
		while (p != -1)
		{
			if (p < 0 || p >= boneCount) break;
			isAncestor[p] = 1;
			p = pData->Get_BoneParentIndex(p);
		}
	}
	
	ImDrawList* dl = ImGui::GetBackgroundDrawList();
	
	// 게임뷰 영역 밖으로 그려지는 거 방지(진짜 중요)
	dl->PushClipRect(vpPos, ImVec2(vpPos.x + vpSize.x, vpPos.y + vpSize.y), true);
	
	vector<ImVec2> screen(boneCount);
	vector<bool>   ok(boneCount, false);
	
	// 본 월드 위치 투영
	for (int i = 0; i < boneCount; ++i)
	{
		// boneWorld = boneModel * ownerWorld (네 컨벤션에 따라 뒤집힐 수도 있음!)
		Matrix boneWorld = boneModel[i] * ownerWorld;
	
		Vector3 pos(boneWorld._41, boneWorld._42, boneWorld._43);
		ok[i] = WorldToScreen(pos, view, proj, vpPos, vpSize, screen[i]);
	}
	
	// 라인(부모-자식)
	for (int i = 0; i < boneCount; ++i)
	{
		const int parent = pData->Get_BoneParentIndex(i);
		if (parent < 0) continue;
		if (!ok[i] || !ok[parent]) continue;
	
		const bool sel = (i == m_iSelectedBone);
		const bool anc = (!sel && isAncestor[i]);
	
		ImU32 col = IM_COL32(255, 230, 90, 255);        // 기본: 노랑
		if (anc) col = IM_COL32(255, 200, 80, 255);     // 조상: 조금 진한 노랑
		if (sel) col = IM_COL32(255, 120, 120, 255);    // 선택: 빨강
	
		float thick = m_fLineThickness + (sel ? 1.2f : 0.f);
	
		dl->AddLine(screen[i], screen[parent], col, thick);
	}
	
	// 관절 점 + 이름
	for (int i = 0; i < boneCount; ++i)
	{
		if (!ok[i]) continue;
	
		const bool sel = (i == m_iSelectedBone);
		const bool anc = (!sel && isAncestor[i]);
	
		if (m_bDrawJoints)
		{
			ImU32 col = IM_COL32(90, 220, 255, 255);     // 기본: 하늘색
			if (anc) col = IM_COL32(255, 220, 120, 255); // 조상: 연노랑
			if (sel) col = IM_COL32(255, 120, 120, 255); // 선택: 빨강
	
			float r = m_fJointRadius + (sel ? 1.5f : 0.f);
			dl->AddCircleFilled(screen[i], r, col);
		}
	
		if (m_bDrawNames)
		{
			ImU32 tcol = sel ? IM_COL32(255, 140, 140, 255) : IM_COL32(255, 255, 255, 220);
			dl->AddText(ImVec2(screen[i].x + 4, screen[i].y - 6), tcol, boneNames[i].c_str());
		}
	}
	
	dl->PopClipRect();
	return;
}

CDebugBonePanel* CDebugBonePanel::Create(GUI_CONTEXT* context)
{
	CDebugBonePanel* instance = new CDebugBonePanel(context);
	if (FAILED(instance->Initialize()))
	{
		Safe_Release(instance);
	}
	return instance;
}

void CDebugBonePanel::Free()
{
	__super::Free();
}

