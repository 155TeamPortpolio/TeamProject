#include "pch.h"
#include "CamObject.h"
#include "Light.h"
#include "GameInstance.h"

HRESULT CCamObject::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CCamera>();
	return S_OK;
}

HRESULT CCamObject::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	return S_OK;
}