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

CComponent* CAnimation3DEX::Clone()
{
	return new CAnimation3DEX(*this);
}

void CAnimation3DEX::Free()
{
	__super::Free();
}
