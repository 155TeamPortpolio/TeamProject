#include "pch.h"
#include "UI_WipeoutRTV.h"

#include "GameInstance.h"
#include "ObjectContainer.h" 
#include "Sprite2D.h"
#include "Shader.h"
#include "Renderer.h"

#include "RectModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"

HRESULT CUI_WipeoutRTV::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CObjectContainer>();
    Add_Component<CRectModel>();
    Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CUI_WipeoutRTV::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("wipeout_test.json")));
    Cache();

    //Set_Alpha(m_isVisible? 1.f : 0.f);

    ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();
    CMaterial* pMaterial = Get_Component<CMaterial>();
    m_pMtrlInst = CMaterialInstance::Create_Handle("wipeout", "UI", pDevice);
    pMaterial->Insert_MaterialInstance(m_pMtrlInst, nullptr);
    auto MaterialDat = m_pMtrlInst->Get_MaterialData();
    if (MaterialDat)
        MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_Mesh.hlsl");

    auto pSprite = Get_Component<CSprite2D>();

    // 렌더타겟 생성
    RenderTargetDesc desc = {};
    desc.Key = "wipeout";
    desc.Width = m_WinSize.x;
    desc.Height = m_WinSize.y;
    RenderSystem()->Create_RenderTarget(desc);

    // 렌더타겟에 SRV 바인딩
    //SHADER_PARAM param = {};
    //auto pSRV = RenderSystem()->Get_CustomTargetSRV("wipeout");
    //param.pData = pSRV;
    //param.typeName = "Texture2D";
    //param.iSize = 0;
    //pSprite->Set_Param("SpriteTexture", param);

    // 뷰, 프로젝션 행렬 구성
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(m_WinSize.x, m_WinSize.y, 0.f, 1.f));

    // 자식 레이어 커스텀으로
    for (auto& pChild : Get_Component<CObjectContainer>()->Get_Children())
        pChild->SetRenderLayer(RENDER_LAYER::CustomOnly);

	return S_OK;
}

void CUI_WipeoutRTV::Awake()
{
}

void CUI_WipeoutRTV::Update(_float dt)
{
    if (InputDevice()->Key_Tap('I'))
    {
        m_isVisible = !m_isVisible;
        Set_Alpha(m_isVisible ? 1.f : 0.f);
        Set_ChildAnimation(CHILD::RAINBOW, 0, true);
    }
    
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);

    // 렌더 타겟에 브러쉬로 그림
    RENDER_CUSTOM_COMMAND command = {};
    command.TargetKey = "wipeout";
    command.bClear = true;
    command.DrawCallback = [this](ID3D11DeviceContext* pContext) { Render_RT(pContext); };
    RenderSystem()->Add_RenderCommand(command, CUSTOMTARGET::UI);

    m_pMtrlInst->Set_Param("g_WorldMatrix", { Get_Component<CTransform>()->Get_WorldMatrix_Ptr(), "float4x4", sizeof(_float4x4) });
}

void CUI_WipeoutRTV::Render_RT(ID3D11DeviceContext* pContext)
{
    auto& children = Get_Component<CObjectContainer>()->Get_Children();
    for (auto& pChild : children)
    {
        if (!pChild)
            continue;

        auto pSprite = pChild->Get_Component<CSprite2D>();
        if (!pSprite)
            continue;

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
}

void CUI_WipeoutRTV::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        auto pObj = pContainer->Find_Descendant(INSTANCENAMES[i]);
        if (!pObj)
            continue;

        m_pChildren[i] = dynamic_cast<CUI_Object*>(pObj);
    }
}

void CUI_WipeoutRTV::Set_ChildAnimation(CHILD child, _int iIndex, _bool isPlayChild)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Animation(0);
    if (!isPlayChild)
        return;

    auto children = pChild->Get_Component<CObjectContainer>()->Get_Children();
    for (auto& pChild : children)
        dynamic_cast<CUI_Object*>(pChild)->Set_Animation(iIndex);
}

CGameObject* CUI_WipeoutRTV::Create()
{
    CUI_WipeoutRTV* pInstance = new CUI_WipeoutRTV();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_WipeoutRTV");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_WipeoutRTV::Clone(INIT_DESC* pArg)
{
    CUI_WipeoutRTV* pInstance = new CUI_WipeoutRTV(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_WipeoutRTV");
        Safe_Release(pInstance);
    }
    return pInstance;
}