#include "Engine_Defines.h"
#include "DebugBonePanel.h"
#include "Animator3D.h"
#include "GameObject.h"
#include "SkeletalModel.h"
#include "ModelData.h"
#include "GameInstance.h"
#include "StaticModel.h"

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

	CGameObject* pTarget = m_pContext->pSelectedObject;
	if (!pTarget) return;

	CSkeletalModel* pSkeletalModel = pTarget->Get_Component<CSkeletalModel>();
	CStaticModel* pStaticModel = pTarget->Get_Component<CStaticModel>();
	CModelData* pData = nullptr;

	if (!pSkeletalModel) {
		if (pStaticModel) {
			pData = pStaticModel->Get_ModelData();
			if (!pData->isSkinned())
				return;
		}
		else
		{
			return;
		}
	}
	else {
		pData = pSkeletalModel->Get_ModelData();
	}

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

	if (!m_bExpanded)
	{
		flags |= ImGuiWindowFlags_NoResize;
		ImGui::SetNextWindowSize(ImVec2(180, 60), ImGuiCond_Always);
	}
	else
	{
		ImGui::SetNextWindowSize(ImVec2(420, 520), ImGuiCond_Always);
	}


	ImGui::Begin("BonePanel", nullptr, flags);

	if (!m_bExpanded)
	{
		if (ImGui::Button("Show Bones", ImVec2(-1, 0)))
			m_bExpanded = true;
		ImGui::End();
		return;
	}

	// 펼쳐진 상태: 상단에 닫기 버튼
	if (ImGui::Button("Hide"))
		m_bExpanded = false;

	ImGui::Checkbox("Names", &m_bDrawNames);
	ImGui::SameLine();
	ImGui::Checkbox("Joints", &m_bDrawJoints);
	ImGui::SameLine();
	ImGui::Checkbox("Only Select", &m_bOnlySelect);
	ImGui::SameLine();
	ImGui::Checkbox("ShowMatrix", &m_bShowMatrix);

	ImGui::SliderFloat("Line Thickness", &m_fLineThickness, 1.0f, 4.0f, "%.1f");
	ImGui::SliderFloat("Joint Radius", &m_fJointRadius, 1.0f, 8.0f, "%.1f");

	if (!pData)
	{
		ImGui::TextDisabled("Empty Model Data");
		ImGui::End();
		return;
	}

	const auto& bones = pData->Get_BoneNames();

	vector<uint8_t> isAncestor(bones.size(), 0);
	if (m_iSelectedBone >= 0 && m_iSelectedBone < (int)bones.size())
	{
		int parent = pData->Get_BoneParentIndex(m_iSelectedBone);
		while (parent != -1)

		{
			if (parent < 0 || parent >= (int)bones.size()) break;
			isAncestor[parent] = 1;
			parent = pData->Get_BoneParentIndex(parent);
		}
	}

	ImGui::SeparatorText("Bones");
	ImGui::BeginChild("##BoneList", ImVec2(0, 220), true);

	for (int i = 0; i < (int)bones.size(); ++i)
	{
		const _bool selected = (m_iSelectedBone == i);

		ImVec4 textCol = ImGui::GetStyleColorVec4(ImGuiCol_Text);
		if (selected)              textCol = ImVec4(1.f, 0.35f, 0.35f, 1.f);
		else if (isAncestor[i])    textCol = ImVec4(1.f, 0.9f, 0.25f, 1.f);

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
	if (m_bShowMatrix) {
		if (CAnimator3D* pAnimator = pTarget->Get_Component<CAnimator3D>()) {
			const _float4x4& nowTrans = pAnimator->Get_CombinedBoneMatrices()[m_iSelectedBone];

			ImGui::SeparatorText("Bone Matrix (Combined)");
			ImGui::BeginChild("##MatView", ImVec2(0, 110), true);

			ImGui::Text("% .4f % .4f % .4f % .4f", nowTrans._11, nowTrans._12, nowTrans._13, nowTrans._14);
			ImGui::Text("% .4f % .4f % .4f % .4f", nowTrans._21, nowTrans._22, nowTrans._23, nowTrans._24);
			ImGui::Text("% .4f % .4f % .4f % .4f", nowTrans._31, nowTrans._32, nowTrans._33, nowTrans._34);
			ImGui::Text("% .4f % .4f % .4f % .4f", nowTrans._41, nowTrans._42, nowTrans._43, nowTrans._44);

			_vector4 MoveTrans = pAnimator->Get_MoveBoneMotionDelta();
			_vector3 move(MoveTrans.x, MoveTrans.y, MoveTrans.z);
			_vector3 dir = move;
			dir.Normalize();

			ImGui::Separator();
			ImGui::Text("MoveBone Movement");
			ImGui::Text("% .4f % .4f % .4f % .4f", MoveTrans.x, MoveTrans.y, MoveTrans.z, MoveTrans.w);
			ImGui::Text("MoveBone Direction");
			ImGui::Text("% .4f % .4f % .4f ", dir.x, dir.y, dir.z);
			
			//RootDelta 추가할것
			_vector3 RootTrans = pAnimator->Get_RootMotionDelta();
			_float RootMoveAmount = XMVectorGetX(XMVector3Length(RootTrans));
			_vector3 RootMove(RootTrans.x, RootTrans.y, RootTrans.z);
			_vector3 RootDir = RootMove;
			RootDir.Normalize();

			ImGui::Separator();
			ImGui::Text("RootBone Movement");
			ImGui::Text("% .4f % .4f % .4f | % .4f ", RootTrans.x, RootTrans.y, RootTrans.z, RootMoveAmount);
			ImGui::Text("RootBone Direction");
			ImGui::Text("% .4f % .4f % .4f ", RootDir.x, RootDir.y, RootDir.z);

			ImGui::EndChild();
		}
	}

	ImVec2 vpPos = ImGui::GetMainViewport()->Pos;
	ImVec2 vpSize = ImGui::GetMainViewport()->Size;

	DrawSkeletonOverlay_ImGui(pTarget, vpPos, vpSize);

	ImGui::End();
}

void CDebugBonePanel::Update_Panel(_float dt)
{


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
		int parent = pData->Get_BoneParentIndex(m_iSelectedBone);
		while (parent != -1)
		{
			if (parent < 0 || parent >= boneCount) break;
			isAncestor[parent] = 1;
			parent = pData->Get_BoneParentIndex(parent);
		}
	}

	ImDrawList* drawList = ImGui::GetBackgroundDrawList();
	drawList->PushClipRect(vpPos, ImVec2(vpPos.x + vpSize.x, vpPos.y + vpSize.y), true);

	vector<ImVec2> screen(boneCount);
	vector<_bool>   isOk(boneCount, false);

	for (int i = 0; i < boneCount; ++i)
	{
		Matrix boneWorld = boneModel[i] * ownerWorld;

		Vector3 pos(boneWorld._41, boneWorld._42, boneWorld._43);
		isOk[i] = WorldToScreen(pos, view, proj, vpPos, vpSize, screen[i]);
	}

	for (int i = 0; i < boneCount; ++i)
	{
		const int parent = pData->Get_BoneParentIndex(i);
		if (parent < 0) continue;
		if (!isOk[i] || !isOk[parent]) continue;

		const bool sel = (i == m_iSelectedBone);
		const bool anc = (!sel && isAncestor[i]);

		ImU32 color = m_bOnlySelect ? IM_COL32(0, 0, 0, 0) : IM_COL32(255, 230, 90, 255);
		if (anc) color = IM_COL32(255, 200, 80, 255);
		if (sel) color = IM_COL32(255, 120, 120, 255);

		float thick = m_fLineThickness + (sel ? 1.2f : 0.f);

		drawList->AddLine(screen[i], screen[parent], color, thick);
	}

	for (int i = 0; i < boneCount; ++i)
	{
		if (!isOk[i]) continue;

		const _bool sel = (i == m_iSelectedBone);
		const _bool anc = (!sel && isAncestor[i]);

		if (m_bDrawJoints)
		{
			ImU32 color = IM_COL32(90, 220, 255, 255);     // 기본: 하늘색
			if (anc) color = IM_COL32(255, 220, 120, 255); // 조상: 연노랑
			if (sel) color = IM_COL32(255, 120, 120, 255); // 선택: 빨강

			float radius = m_fJointRadius + (sel ? 1.5f : 0.f);
			drawList->AddCircleFilled(screen[i], radius, color);
		}

		if (m_bDrawNames)
		{
			if (m_bOnlySelect && !sel)
				continue;

			ImU32 tcol = sel ? IM_COL32(255, 140, 140, 255)
				: IM_COL32(255, 255, 255, 220);

			drawList->AddText(ImVec2(screen[i].x + 4, screen[i].y - 6), tcol, boneNames[i].c_str());
		}
	}

	drawList->PopClipRect();
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

