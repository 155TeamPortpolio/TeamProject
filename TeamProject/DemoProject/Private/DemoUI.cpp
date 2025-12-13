#include "pch.h"
#include "DemoUI.h"
#include "TextSlot.h"
#include "Sprite2D.h"
#include "GameInstance.h"

CDemoUI::CDemoUI()
{
}

CDemoUI::CDemoUI(const CDemoUI& rhs)
	:CUI_Object(rhs)
{
}

HRESULT CDemoUI::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	auto pRcsMgr = CGameInstance::GetInstance()->Get_ResourceMgr();
	pRcsMgr->Add_ResourcePath("testUI.png", "../../DemoResource/ui/testUI.png");

	Add_Component<CTextSlot>();
	Get_Component<CTextSlot>()->Set_Color(_float4(0.447, 0.365, 0.259, 1.0));
	Get_Component<CTextSlot>()->Set_Font("Sindy");
	Get_Component<CTextSlot>()->Set_Size(0.7);

	Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
	Get_Component<CSprite2D>()->Add_Texture(G_GlobalLevelKey, "testUI.png");
	return S_OK;
}

HRESULT CDemoUI::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	return S_OK;
}

void CDemoUI::Awake()
{
}

void CDemoUI::Priority_Update(_float dt)
{
}

void CDemoUI::Update(_float dt)
{
	Get_Component<CTextSlot>()->Set_Text(L"Hello");
	Get_Component<CTextSlot>()->Push_Text();
}

void CDemoUI::Late_Update(_float dt)
{
}

void CDemoUI::Render_GUI()
{
	__super::Render_GUI();
}

CDemoUI* CDemoUI::Create()
{
	CDemoUI* instance = new CDemoUI();
	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Object Create Failed : CDemoUI");
		Safe_Release(instance);
	}

	return instance;
}

CGameObject* CDemoUI::Clone(INIT_DESC* pArg)
{
	CDemoUI* instance = new CDemoUI(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		MSG_BOX("Object Clone Failed : CDemoUI");
		Safe_Release(instance);
	}

	return instance;
}

void CDemoUI::Free()
{
	__super::Free();
}