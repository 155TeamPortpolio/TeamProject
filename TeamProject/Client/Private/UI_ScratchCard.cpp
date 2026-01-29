#include "pch.h"
#include "UI_ScratchCard.h"

#include "GameInstance.h" 
#include "Renderer.h"
#include "Shader.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "UIDirector.h"
#include "UI_Lottery.h"

HRESULT CUI_ScratchCard::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_ScratchCard::Initialize(INIT_DESC* pArg)
{
    SCRATCH_DESC* pDesc = static_cast<SCRATCH_DESC*>(pArg);
    m_pState = pDesc->pState;
    m_onScratchCompleted = pDesc->onScratchCompleted;

    __super::Initialize(pArg);

    // JSON 로드
    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("scratchCard.json")));

    // 캐싱 (오브젝트, 컴포넌트)
    Cache();

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
    auto pBrush = m_pChildren[ENUM(CHILD::BRUSH)];
    if(pBrush)
        pBrush->SetRenderLayer(RENDER_LAYER::CustomOnly);

    // 뷰, 프로젝션 행렬 구성
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(m_vSize.x, m_vSize.y, 0.f, 1.f));

    Set_Alive(false);

    return S_OK;
}

void CUI_ScratchCard::Update(_float dt)
{
    // 브러쉬 자식 객체의 위치를 마우스 위치로
    auto pBrush = m_pChildren[ENUM(CHILD::BRUSH)];
    if(pBrush && InputDevice()->Mouse_Hold(MOUSE_BTN::LB))
        pBrush->Set_AnchorOffset(InputDevice()->Mouse_Pos() - m_vLeftTop);

    // 스크래치가 ~퍼센트 이상이면 상태 변경
    if (!m_isScratchComplete && Check_Scratch(dt))
    {
        m_isScratchComplete = true;
        if (m_onScratchCompleted)
            m_onScratchCompleted(); 
    } 

    // 스크래치가 완료되고, 결과를 한 번 보여줌
    if(m_isScratchComplete && !m_hasShownResult)
    { 
        m_fResultWaitTime += dt;
        
        if (m_fResultWaitTime >= m_fResultWaitDuration)
        { 
            UIDirector()->Show_ResultBanner(REWARD_TEXTURES[0], L"1234", L"보상 아이템 이름");
            m_hasShownResult = true;
        }
    }

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

    if(*m_pState == CUI_Lottery::STATE::READY)
        Reset();
}

void CUI_ScratchCard::UI_DeActive(void* pArg)
{
    Change_State(STATE::INVISIBLE);
}

void CUI_ScratchCard::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        auto pObj = pContainer->Find_Descendant(INSTANCENAMAES[i]);
        if (!pObj)
            continue;

        m_pChildren[i] = dynamic_cast<CUI_Object*>(pObj);
        m_pSprites[i] = pObj->Get_Component<CSprite2D>();
    }
}

_bool CUI_ScratchCard::Check_Scratch(_float dt)
{
    m_fScratchTimer += dt;

    if (m_fScratchTimer < m_fScratchDuration)
        return false;

    m_fScratchTimer = 0.f;
    return Calculate_ScratchRatio() >= m_fScratchRatio;
}

_float CUI_ScratchCard::Calculate_ScratchRatio()
{
    auto pContext = GameInstance()->Get_Context();
    auto pTexture = RenderSystem()->Get_CustomTargetTexture("scratchCard");

    D3D11_TEXTURE2D_DESC desc = {};
    pTexture->GetDesc(&desc);

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    pContext->Map(pTexture, 0, D3D11_MAP_READ, 0, &mapped);
    BYTE* pData = (BYTE*)mapped.pData;
    _uint rowPitch = mapped.RowPitch;

    _uint iWhiteCount = {};
    _uint iTotalCount = desc.Width * desc.Height;

    for (_uint j = 0; j < desc.Height; ++j)
    {
        BYTE* pRow = pData + j * rowPitch;

        for (_uint i = 0; i < desc.Width; ++i)
        {
            BYTE* pPixel = pRow + i * 4;

            if (pPixel[0] >= 200.f)
                ++iWhiteCount;
        }
    }

    pContext->Unmap(pTexture, 0);
    Safe_Release(pTexture);

    return iWhiteCount / static_cast<_float>(iTotalCount);
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
        break;
    }
}

void CUI_ScratchCard::Change_RewardTexture(const string& strTextureKey)
{
    if (!m_pSprites[ENUM(CHILD::REWARD)])
        return;

    m_pSprites[ENUM(CHILD::REWARD)]->Change_Texture(0, G_GlobalLevelKey, strTextureKey);
}

void CUI_ScratchCard::Reset()
{
    m_isClear = true;
    m_isScratchComplete = false;
    m_hasShownResult = false;
    Change_RewardTexture(REWARD_TEXTURES[rand() % ENUM(REWARD::END)]); 
}

void CUI_ScratchCard::Render_RTBrush(ID3D11DeviceContext* pContext)
{ 
    CHILD child = (m_isScratchComplete)? CHILD::SCRATCH : CHILD::BRUSH;

    Render_RT(child, pContext);
}

void CUI_ScratchCard::Render_RT(CHILD child, ID3D11DeviceContext* pContext)
{
    auto pChild = m_pChildren[ENUM(child)];
    auto pSprite = m_pSprites[ENUM(child)];

    if (!pChild || !pSprite)
        return;

    auto pBrushShader = pSprite->Get_Shader();

    ID3D11InputLayout* pLayout;
    RenderSystem()->GetRenderer(RENDERER_TYPE::UI)->Get_BufferInputLayout(pSprite->Get_Buffer(), pBrushShader, "OpaqueCustom", &pLayout);
    pContext->IASetInputLayout(pLayout);
    pSprite->Set_Param("g_WorldMatrix", { pChild->Get_Component<CTransform>()->Get_WorldMatrix_Ptr(), "matrix", sizeof(_float4x4) });
    pSprite->Set_Param("g_ViewMatrix", { &m_ViewMatrix, "matrix", sizeof(_float4x4) });
    pSprite->Set_Param("g_ProjMatrix", { &m_ProjMatrix, "matrix", sizeof(_float4x4) });
    pSprite->Apply_Shader(pContext);
    pSprite->Set_Param("vColor", { &m_vColor, "float4", sizeof(_float4) });

    pBrushShader->Apply("OpaqueCustom", pContext);
    pSprite->Draw_Sprite(pContext);
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

void CUI_ScratchCard::Free()
{
    __super::Free();
}