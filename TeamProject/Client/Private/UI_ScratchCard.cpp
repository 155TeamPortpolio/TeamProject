#include "pch.h"
#include "UI_ScratchCard.h"

#include "GameInstance.h" 
#include "Renderer.h"
#include "Shader.h"
#include "PipeLine.h"
#include "ObjectContainer.h"
#include "PointModel.h"
#include "Sprite2D.h"

HRESULT CUI_ScratchCard::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_ScratchCard::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");

    // JSON 로드
    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("scratchCard.json")));

    // 캐싱
    Cache_Brush();
    Cache_Reward();

    // 브러쉬 렌더 레이어 커스텀으로
    m_pBrush->SetRenderLayer(RENDER_LAYER::CustomOnly);

    // 렌더타겟 생성
    RenderTargetDesc desc = {};
    desc.Key = "scratchCard";
    desc.Width = m_WinSize.x;
    desc.Height = m_WinSize.y;
    desc.vClearColor = { 1.f, 0.f, 0.f, 1.f };
    RenderSystem()->Create_RenderTarget(desc);

    // 뷰, 프로젝션 행렬 구성
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(m_WinSize.x, m_WinSize.y,  0.f, 1.f));

	return S_OK;
}

void CUI_ScratchCard::Awake()
{
}

void CUI_ScratchCard::Update(_float dt)
{
    __super::Update(dt);

    // 브러쉬 자식 객체의 위치를 마우스 위치로 (근데 앵커 오프셋인데)
    auto pMouse = InputDevice()->Mouse_Pos();
    m_pBrush->Set_AnchorOffset(pMouse - m_vLeftTop);

    Get_Component<CObjectContainer>()->UpdateChild(dt); 
}

void CUI_ScratchCard::Late_Update(_float dt)
{
    // 렌더 타겟에 그림
    RENDER_CUSTOM_COMMAND command = {};
    command.TargetKey = "scratchCard";
    command.bClear = false;
    command.DrawCallback = [this](ID3D11DeviceContext* pContext) { Render_Scratch(pContext); };
    RenderSystem()->Add_RenderCommand(command, CUSTOMTARGET::UI);

    SHADER_PARAM param = {};
    param.pData = RenderSystem()->Get_CustomTargetSRV("scratchCard");
    param.typeName = "Texture2D";
    param.iSize = 0;
    Get_Component<CSprite2D>()->Set_Param("SpriteTexture", param);
}

void CUI_ScratchCard::UI_Active(void* pArg)
{
    Change_RewardTexture(REWARD_TEXTURES[rand() % ENUM(REWARD::END)]);
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

void CUI_ScratchCard::Change_RewardTexture(const string& strTextureKey)
{
    if (!m_pRewardSprite)
        return;

    m_pRewardSprite->Change_Texture(0, G_GlobalLevelKey, strTextureKey);
}

void CUI_ScratchCard::Render_Scratch(ID3D11DeviceContext* pContext)
{ 
    // 브러쉬 객체를 렌더 타겟에 그림
    auto pShader = m_pBrushSprite->Get_Shader();

    ID3D11InputLayout* pLayout;
    RenderSystem()->GetRenderer(RENDERER_TYPE::UI)->Get_BufferInputLayout(m_pBrushSprite->Get_Buffer(), pShader, "OpaqueCustom", &pLayout);

    pContext->IASetInputLayout(pLayout);

    m_pBrushSprite->Set_Param("g_WorldMatrix", { m_pBrush->Get_Component<CTransform>()->Get_WorldMatrix_Ptr(), "matrix", sizeof(_float4x4)});
    m_pBrushSprite->Set_Param("g_ViewMatrix", { &m_ViewMatrix, "matrix", sizeof(_float4x4) });
    m_pBrushSprite->Set_Param("g_ProjMatrix", { &m_ProjMatrix, "matrix", sizeof(_float4x4) });
    m_pBrushSprite->Apply_Shader(pContext);
    m_pBrushSprite->Set_Param("vColor", { &m_vColor, "float4", sizeof(_float4) });

    pShader->Apply("OpaqueCustom", pContext);
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