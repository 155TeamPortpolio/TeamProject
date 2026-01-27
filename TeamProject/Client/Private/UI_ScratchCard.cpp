#include "pch.h"
#include "UI_ScratchCard.h"

#include "GameInstance.h" 
#include "Renderer.h"
#include "Shader.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "UIDirector.h"

HRESULT CUI_ScratchCard::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_ScratchCard::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    // JSON 로드
    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("scratchCard.json")));

    // 캐싱 (오브젝트, 컴포넌트)
    Cache_Brush();
    Cache_Reward();

    // 셰이더, 패스 설정
    auto pSprite = Get_Component<CSprite2D>();
    pSprite->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    pSprite->ChangePass("UI_StencilWrite");
    pSprite->Set_Param("MaskThreshold", { &m_fThreshold, "float", sizeof(_float) });

    // 렌더타겟 생성
    RenderTargetDesc desc = {};
    desc.Key = "scratchCard";
    desc.Width = m_vSize.x;
    desc.Height = m_vSize.y;
    RenderSystem()->Create_RenderTarget(desc);

    // 렌더타겟에 SRV 바인딩
    SHADER_PARAM param = {};
    auto pSRV = RenderSystem()->Get_CustomTargetSRV("scratchCard");
    param.pData = pSRV;
    param.typeName = "Texture2D";
    param.iSize = 0;
    pSprite->Set_Param("SpriteTexture", param);

    // 브러쉬 렌더 레이어 커스텀으로
    m_pBrush->SetRenderLayer(RENDER_LAYER::CustomOnly);

    // 뷰, 프로젝션 행렬 구성
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(m_vSize.x, m_vSize.y, 0.f, 1.f));

    Set_Alive(false);

    return S_OK;
}

void CUI_ScratchCard::Update(_float dt)
{
    if (InputDevice()->Key_Tap('R'))    // 임시로 나중에 다 채워지면 넘어가게
        UIDirector()->Show_ResultBanner(REWARD_TEXTURES[0], L"결과는", L"이러이러하다");
        
    // 브러쉬 자식 객체의 위치를 마우스 위치로
    m_pBrush->Set_AnchorOffset(InputDevice()->Mouse_Pos() - m_vLeftTop);

    __super::Update(dt);
     
    Get_Component<CObjectContainer>()->UpdateChild(dt); 

    if (m_eState == STATE::VISIBLE && Is_AnimFinished())
        Set_Animation(ENUM(ANIMATION::IDLE));

    if (m_eState == STATE::INVISIBLE && Is_AnimFinished())
        Set_Alive(false);

    // 렌더 타겟에 브러쉬로 그림
    RENDER_CUSTOM_COMMAND command = {};
    command.TargetKey = "scratchCard";
    command.bClear = m_isClear;
    m_isClear = false;
    command.DrawCallback = [this](ID3D11DeviceContext* pContext) { Render_RTBrush(pContext); };
    RenderSystem()->Add_RenderCommand(command, CUSTOMTARGET::UI);
}

void CUI_ScratchCard::UI_Active(void* pArg)
{
    Change_State(STATE::VISIBLE);
}

void CUI_ScratchCard::UI_DeActive(void* pArg)
{
    Change_State(STATE::INVISIBLE);
}

void CUI_ScratchCard::Cache_Brush()
{
    auto pContainer = Get_Component<CObjectContainer>();

    auto pObj = pContainer->Find_Descendant("brush");
    if (!pObj)
        return;

    m_pBrush = dynamic_cast<CUI_Object*>(pObj);
    m_pBrushSprite = pObj->Get_Component<CSprite2D>();
}

void CUI_ScratchCard::Cache_Reward()
{
    auto pContainer = Get_Component<CObjectContainer>();

    auto pObj = pContainer->Find_Descendant("reward");
    if (!pObj)
        return;

    m_pRewardSprite = pObj->Get_Component<CSprite2D>();
}

void CUI_ScratchCard::Change_State(STATE eState)
{
    if (m_eState == eState)
        return;

    m_eState = eState;
    switch (eState)
    {
    case STATE::INVISIBLE:
        Set_Animation(ENUM(ANIMATION::DISAPPEAR));
        break;
    case STATE::VISIBLE:
        Set_Alive(true);
        Set_Animation(ENUM(ANIMATION::APPEAR));
        Change_RewardTexture(REWARD_TEXTURES[rand() % ENUM(REWARD::END)]);
        m_isClear = true;
        break;
    }
}

void CUI_ScratchCard::Change_RewardTexture(const string& strTextureKey)
{
    if (!m_pRewardSprite)
        return;

    m_pRewardSprite->Change_Texture(0, G_GlobalLevelKey, strTextureKey);
}

void CUI_ScratchCard::Render_RTBrush(ID3D11DeviceContext* pContext)
{ 
    // 브러쉬를 렌더 타겟에 그림
    auto pBrushShader = m_pBrushSprite->Get_Shader();

    ID3D11InputLayout* pLayout;
    RenderSystem()->GetRenderer(RENDERER_TYPE::UI)->Get_BufferInputLayout(m_pBrushSprite->Get_Buffer(), pBrushShader, "OpaqueCustom", &pLayout);
    pContext->IASetInputLayout(pLayout);
    m_pBrushSprite->Set_Param("g_WorldMatrix", { m_pBrush->Get_Component<CTransform>()->Get_WorldMatrix_Ptr(), "matrix", sizeof(_float4x4)});
    m_pBrushSprite->Set_Param("g_ViewMatrix", { &m_ViewMatrix, "matrix", sizeof(_float4x4) });
    m_pBrushSprite->Set_Param("g_ProjMatrix", { &m_ProjMatrix, "matrix", sizeof(_float4x4) });
    m_pBrushSprite->Apply_Shader(pContext);
    m_pBrushSprite->Set_Param("vColor", { &m_vColor, "float4", sizeof(_float4) });

    pBrushShader->Apply("OpaqueCustom", pContext);
    m_pBrushSprite->Draw_Sprite(pContext);
}

CGameObject* CUI_ScratchCard::Create()
{
    CUI_ScratchCard* pInstance = new CUI_ScratchCard();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_ScratchCard");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_ScratchCard::Clone(INIT_DESC* pArg)
{
    CUI_ScratchCard* pInstance = new CUI_ScratchCard(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_ScratchCard");
        Safe_Release(pInstance);
    }
    return pInstance;
}