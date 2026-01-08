#include "pch.h"
#include "UI_Decibel.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "Texture.h"

const CUI_Decibel::tagUIDesc CUI_Decibel::UIDescs[] =
{
    { KANJI,      "CombatUproar.png", 50.f,   0.f, END },
    { DIGIT_1000, "00.png", 30.f,    0.f,  KANJI },
    { DIGIT_100,  "01.png", 30.f,     0.f,  DIGIT_1000 },
    { DIGIT_10,   "02.png", 30.f,   0.f,   DIGIT_100 },
    { DIGIT_1,    "03.png", 30.f ,   0.f,    DIGIT_10 },
    { POINT,      "CombatPTS.png", 20.f,  12.f,  DIGIT_1 },
};

HRESULT CUI_Decibel::Initialize_Prototype()
{
    __super::Initialize_Prototype();

	return S_OK;
}

HRESULT CUI_Decibel::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Add_Component<CObjectContainer>();

    auto pGameInstance = CGameInstance::GetInstance();
    
    // ³ôÀÌ 50¿¡ ¸ÂÃç // ³ôÀÌ 30¿¡ ¸ÂÃç
    string strCurrentLevel = pGameInstance->Get_LevelMgr()->Get_NowLevelKey();
    for (_int i = 0; i < UI::END; ++i)
    {
        CUI_Object* uiObj = Builder::Create_UIObject({ strCurrentLevel, "Proto_GameObject_Image" })
            .Offset(_float2(i * 200.f, 0.f))
            .Build("Image");
    
        uiObj->Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, CUI_Decibel::UIDescs[i].strTextureTag);
        _float fRatioAspect = uiObj->Get_Component<CSprite2D>()->Get_CurTexture()->Get_AspectRatio();
        uiObj->Set_Size(_float2(UIDescs[i].fHeight * fRatioAspect, UIDescs[i].fHeight));

        Get_Component<CObjectContainer>()->Add_Child(uiObj);
    }

	return S_OK;
}

void CUI_Decibel::Awake()
{
}

void CUI_Decibel::Update(_float dt)
{
}

CGameObject* CUI_Decibel::Create()
{
    CUI_Decibel* pInstance = new CUI_Decibel();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_DecibelGauge");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Decibel::Clone(INIT_DESC* pArg)
{
    CUI_Decibel* pInstance = new CUI_Decibel(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Decibel");
        Safe_Release(pInstance);
    }
    return pInstance;
}