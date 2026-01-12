#include "pch.h"
#include "MapAnimObject.h"
#include "GameInstance.h"
#include "MapData_Defines.h"
#include "MapLoader_Helper.h"

#include "ModelData.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"

#include "EffectContainer.h"


CMapAnimObject::CMapAnimObject()
{
}

CMapAnimObject::CMapAnimObject(const CMapAnimObject& rhs)
{
}

HRESULT CMapAnimObject::Initialize_Prototype()
{
    return E_NOTIMPL;
}

HRESULT CMapAnimObject::Initialize(INIT_DESC* pArg)
{
    return E_NOTIMPL;
}

void CMapAnimObject::Awake()
{
}

void CMapAnimObject::Priority_Update(_float dt)
{
}

void CMapAnimObject::Update(_float dt)
{
}

void CMapAnimObject::Late_Update(_float dt)
{
}

void CMapAnimObject::Render_GUI()
{
}

CGameObject* CMapAnimObject::Clone(INIT_DESC* pArg)
{
    return nullptr;
}

void CMapAnimObject::Free()
{
    __super::Free();
}