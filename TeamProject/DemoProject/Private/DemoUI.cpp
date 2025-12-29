#include "pch.h"
#include "DemoUI.h"
#include "TextSlot.h"
#include "Sprite2D.h"
#include "GameInstance.h"
#include "EventListener.h"
#include "ObjectContainer.h"

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

	Get_Component<CSprite2D>()->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
	Get_Component<CSprite2D>()->Add_Texture(G_GlobalLevelKey, "testUI.png");

	Add_Component<CEventListener>();
	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CDemoUI::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);
	Get_Component<CEventListener>()->Add_Listner<ExampleEvt>([this](const ExampleEvt& evt) {Listen(evt); });

	return S_OK;
}

void CDemoUI::Listen(const ExampleEvt& evt)
{
	m_vSize = { 20,20 };
}

void CDemoUI::Awake()
{
}

void CDemoUI::Priority_Update(_float dt)
{
}

void CDemoUI::Update(_float dt)
{
}

void CDemoUI::Late_Update(_float dt)
{
}

void CDemoUI::Render_GUI()
{
	__super::Render_GUI();

	if (ImGui::Button("Add Child")) {
		CUI_Object* UI = Builder::Create_UIObject({ "Demo_Level" ,"Proto_GameObject_DemoUI" })
			.Scale({ 500,500 })
			.Offset({ 0,0, })
			.Build("Demo_UI");

		Get_Component<CObjectContainer>()->Add_Child(UI, false);
	}
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