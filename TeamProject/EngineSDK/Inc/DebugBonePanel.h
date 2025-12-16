#pragma once
#include "BasePanel.h"
NS_BEGIN(Engine)
class CDebugBonePanel :
    public CBasePanel
{
private:
	CDebugBonePanel(GUI_CONTEXT* context);
	virtual ~CDebugBonePanel() DEFAULT;
	HRESULT Initialize();

public:
	virtual void Render_GUI() override;
	virtual void Update_Panel(_float dt) override;

private:
	_bool WorldToScreen(
		const _float3& world,
		const _float4x4& view,
		const _float4x4& proj,
		const ImVec2& vpPos,
		const ImVec2& vpSize,
		ImVec2& out) const;

	void DrawSkeletonOverlay_ImGui(
		class CGameObject* target,
		const ImVec2& vpPos,
		const ImVec2& vpSize);

private:
	_bool    m_bEnable = true;
	_bool		m_bExpanded = false;
	_bool    m_bDrawNames = false;
	_bool    m_bDrawJoints = true;
	_bool    m_bOnlySelect = false;
	_float   m_fLineThickness = 1.5f;
	_float   m_fJointRadius = 3.0f;
	_int     m_iSelectedBone = -1;

public:
	static CDebugBonePanel* Create(GUI_CONTEXT* context);
	virtual void Free();
};
NS_END
