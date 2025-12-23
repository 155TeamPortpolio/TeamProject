#include "Animator3DEX.h"
#include "AnimationClip.h"

CAnimator3DEX::CAnimator3DEX()
{
}

CAnimator3DEX::CAnimator3DEX(const CAnimator3DEX& rhs)
	: CAnimator3D(rhs)
{
}

HRESULT CAnimator3DEX::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAnimator3DEX::Initialize(COMPONENT_DESC* pArg)
{
	return S_OK;
}

void CAnimator3DEX::Render_GUI()
{
	//__super::Render_GUI();
}

void CAnimator3DEX::Update_Animation(_float fTrackPosition)
{

	if (m_AnimLayers.empty()) return;
	
	for (auto& Layer : m_AnimLayers) {
		if (-1 == Layer.iClipIndex) continue;
	
		auto& nowClip = m_pAnimClips[Layer.iClipIndex];

		nowClip->Check_Event(Layer.fCurrentTrackPosition, fTrackPosition, m_EventBus);

		Layer.fCurrentTrackPosition = fTrackPosition;
		nowClip->TranslateAnimateMatrixFromDuration(Layer.LocalMatrices, fTrackPosition, m_EventBus);
	}
	
	BuildBone();
}

vector<class CAnimationClip*>* CAnimator3DEX::Get_Clips()
{
	return &m_pAnimClips;
}

vector<CAnimator3D::ANIM_LAYER>& CAnimator3DEX::Get_AnimLayers()
{
	return m_AnimLayers;
}

CAnimator3DEX* CAnimator3DEX::Create()
{
	CAnimator3DEX* instance = new CAnimator3DEX();
	if (FAILED(instance->Initialize_Prototype())) {
		Safe_Release(instance);
	}
	return instance;
}

CComponent* CAnimator3DEX::Clone()
{
	return new CAnimator3DEX(*this);
}

void CAnimator3DEX::Free()
{
	__super::Free();
}
