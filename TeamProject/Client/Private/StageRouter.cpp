#include "pch.h"
#include "StageRouter.h"
#include "Stage.h"
#include "ZeroStage_Boss.h"
#include "ZeroStage_Normal.h"
#include "ZeroStage_Elite.h"

CStageRouter::CStageRouter()
	:CLevelObject()
{
	m_StageQueue.push(StageType::Normal);
	m_StageQueue.push(StageType::Elite);
	m_StageQueue.push(StageType::Boss);
}

void CStageRouter::Render_GUI()
{
	if (ImGui::Button("Change"))
		dynamic_cast<CZero_Level*>(m_pOwner)->ChangeStage();
	if(ImGui::Button("Normal"))
		m_StageQueue.push(StageType::Normal);
	if(ImGui::Button("Elite"))
		m_StageQueue.push(StageType::Elite);
	if(ImGui::Button("Boss"))
		m_StageQueue.push(StageType::Boss);
}

StageType CStageRouter::Pop_StageType()
{
	auto stage = m_StageQueue.front();
	m_StageQueue.pop();
	return stage;
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