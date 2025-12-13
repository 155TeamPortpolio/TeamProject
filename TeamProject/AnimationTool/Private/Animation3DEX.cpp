#include "Animation3DEX.h"

CAnimation3DEX::CAnimation3DEX()
{
}

CAnimation3DEX::CAnimation3DEX(const CAnimation3DEX& rhs)
	: CAnimator3D(rhs)
{
}

CAnimation3DEX* CAnimation3DEX::Create()
{
	CAnimation3DEX* instance = new CAnimation3DEX();
	if (FAILED(instance->Initialize_Prototype())) {
		Safe_Release(instance);
	}
	return instance;
}

CComponent* CAnimator3D::Clone()
{
	return new CAnimator3D(*this);
}

void CAnimator3D::Free()
{
	__super::Free();
}