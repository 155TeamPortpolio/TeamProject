#include "pch.h"
#include "UIPrefab.h"

CUIPrefab::CUIPrefab()
{
}

CUIPrefab::CUIPrefab(const CUIPrefab& rhs)
	: CUI_Object(rhs)
{
}

HRESULT CUIPrefab::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CUIPrefab::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CUIPrefab::Priority_Update(_float dt)
{
}

void CUIPrefab::Update(_float dt)
{
}

void CUIPrefab::Late_Update(_float dt)
{
}

void CUIPrefab::Free()
{
	__super::Free();
}