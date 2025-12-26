#include "pch.h"
#include "CanvasPanel.h"

#include "GameInstance.h"
#include "Helper_Func.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"

#include "ImageUI.h"
#include "TextUI.h"
#include "ButtonUI.h"
#include "SpriteAnimationUI.h"
#include "UVAnimationUI.h"
#include "GaugeUI.h"

_uint CCanvasPanel::m_iCount = {};
const string CCanvasPanel::m_strTypeTag = "CanvasPanel";

CCanvasPanel::CCanvasPanel()
{
}

CCanvasPanel::CCanvasPanel(const CCanvasPanel& rhs)
    : CUIObject_Tool(rhs)
{
}

HRESULT CCanvasPanel::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CCanvasPanel::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

#ifdef _DEBUG
    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    Get_Component<CSprite2D>()->Add_Texture(G_GlobalLevelKey, "canvas.png");
#endif

    m_iCount++;
    
    return S_OK;
}

void CCanvasPanel::Priority_Update(_float dt)
{
    Get_Component<CObjectContainer>()->Priority_UpdateChild(dt);
}

void CCanvasPanel::Update(_float dt)
{
    if (!m_isAlive)
        return;

    Get_Component<CObjectContainer>()->UpdateChild(dt);

    Play_Animation(dt);
}

void CCanvasPanel::Late_Update(_float dt)
{
    Get_Component<CObjectContainer>()->Late_UpdateChild(dt);
}

void CCanvasPanel::Render_GUI()
{
    __super::Render_GUI();
}

void CCanvasPanel::FillElementData(UI_ELEMENT_DATA& data)
{
    __super::FillElementData(data);

    data.strTypeTag = m_strTypeTag;
}

CGameObject* CCanvasPanel::Create()
{
    CCanvasPanel* pInstance = new CCanvasPanel();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CCanvasPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCanvasPanel::Clone(INIT_DESC* pArg)
{
    CCanvasPanel* pInstance = new CCanvasPanel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CCanvasPanel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCanvasPanel::Free()
{
    __super::Free();
}