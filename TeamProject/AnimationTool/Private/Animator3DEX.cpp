#include "Animator3DEX.h"
#include "AnimationClipEX.h"
#include "AnimationLayout.h"
#include "GameInstance.h"

CAnimator3DEX::CAnimator3DEX()
{
}

CAnimator3DEX::CAnimator3DEX(const CAnimator3DEX& rhs)
	: CAnimator3D(rhs)
{
}

HRESULT CAnimator3DEX::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CAnimator3DEX::Initialize(COMPONENT_DESC* pArg)
{
	return E_NOTIMPL;
}

HRESULT CAnimator3DEX::Load_ClipMeta(const string& LevelKey, const string& ClipMetaKey)
{

	//CGameInstance::GetInstance()->Get_ResourceMgr()->Get
	//
	//
	return E_NOTIMPL;
}

CAnimator3DEX* CAnimator3DEX::Create()
{
	CAnimator3DEX* pInstance = new CAnimator3DEX();

	if (FAILED(pInstance->Initialize_Prototype())) {
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CAnimator3DEX::Clone()
{
	return new CAnimator3DEX(*this);
}

void CAnimator3DEX::Free()
{
	__super::Free();
}
