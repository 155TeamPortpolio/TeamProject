#include "pch.h"
#include "UI_Mouse.h"

#include "GameInstance.h"
#include "Sprite2D.h"

HRESULT CUI_Mouse::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Mouse::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    auto sprite = Get_Component<CSprite2D>();
    sprite->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    sprite->Add_Texture(G_GlobalLevelKey, "MouseIcon.png");
    sprite->ChangePass("Opaque");

    Set_Size({ 28.f, 28.f });

    return S_OK;
}

void CUI_Mouse::Awake()
{ 
    Set_Alive(false);
}

void CUI_Mouse::Update(_float dt)
{
    Set_AnchorOffset(InputDevice()->Mouse_Pos() + _float2(-2.f, -1.f));

    __super::Update(dt);
}

void CUI_Mouse::UI_Active(void* pArg)
{
    Set_Alive(true);
}

void CUI_Mouse::UI_DeActive(void* pArg)
{
    Set_Alive(false);
}

CGameObject* CUI_Mouse::Create()
{
    CUI_Mouse* pInstance = new CUI_Mouse();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Mouse");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Mouse::Clone(INIT_DESC* pArg)
{
    CUI_Mouse* pInstance = new CUI_Mouse(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Mouse");
        Safe_Release(pInstance);
    }
    return pInstance;
}