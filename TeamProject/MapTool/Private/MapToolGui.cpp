#include "pch.h"
#include "MapToolGui.h"
#include "GameInstance.h"


CMapToolGui::CMapToolGui(GUI_CONTEXT* pContext)
	: CBasePanel(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMapToolGui::Initialize()
{
	m_pGameInstance->Get_RayMgr()->Register_Ray(&m_Ray);

	return S_OK;
}

void CMapToolGui::Update_Panel(_float dt)
{
    Compute_Ray();

	if (m_pGameInstance->Get_InputDev()->Key_Tap('P')) {
		RAY_HIT* pRayHit = m_pGameInstance->Get_RayMgr()->Get_FrontRayHit();
		m_vRayHitPos = pRayHit->vHittedPosition;
        Place_Object(pRayHit);
	}
}

void CMapToolGui::Render_GUI()
{
	ImGui::PushID(this);

	ImGui::SeparatorText("MapTool Controller");
	
	ImGui::Text("Last Ray Hit Pos : %.3f, %.3f, %.3f ", m_vRayHitPos.x, m_vRayHitPos.y, m_vRayHitPos.z);
    
    ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "Scale");
    ImGui::InputFloat3("##Scale", reinterpret_cast<float*>(&m_vScale_PlacedObject), "%.1f");
    

	ImGui::PopID();
}

void CMapToolGui::Compute_Ray()
{
    m_Ray = {};

    POINT   pt = {};

    GetCursorPos(&pt);
    ScreenToClient(g_hWnd, &pt);

    /*마우스 좌표 변환 */
        _vector vMouseOrigin = XMVectorSet(
            static_cast<float>(pt.x),
            static_cast<float>(pt.y),
            0.f,
            1.f
        );
        _matrix matProj = XMLoadFloat4x4(m_pGameInstance->Get_CameraMgr()->Get_ProjMatrix());
        _matrix matView = XMLoadFloat4x4(m_pGameInstance->Get_CameraMgr()->Get_ViewMatrix());

    /*마우스 레이 변환 */
        _vector raySrc = XMVector3Unproject(
            vMouseOrigin,
            0.0f, 0.0f,
            static_cast<float>(MapTool::g_iWinSizeX),
            static_cast<float>(MapTool::g_iWinSizeY),
            0.0f, 1.0f,
            matProj,
            matView,
            XMMatrixIdentity()
        );

    /*마우스 목적지 좌표*/
        _vector vMouseDest = XMVectorSet(
            static_cast<float>(pt.x),
            static_cast<float>(pt.y),
            1.f,
            1.f
        );

        _vector rayDest = XMVector3Unproject(
        vMouseDest,
        0.0f, 0.0f,
        static_cast<float>(MapTool::g_iWinSizeX),
        static_cast<float>(MapTool::g_iWinSizeY),
        0.0f, 1.0f,
        matProj,
        matView,
        XMMatrixIdentity()
    );

    _vector rayOrigin = raySrc;
    _vector rayDir = XMVector3Normalize(rayDest - raySrc);


    XMStoreFloat3(&m_Ray.vRayDirection, rayDir);
    XMStoreFloat3(&m_Ray.vRayOrigin, rayOrigin);
    m_Ray.fMaxDistance = 1550.f;
}

void CMapToolGui::Place_Object(RAY_HIT* pRayHit)
{
    if (nullptr == pRayHit->pObject)
        return;

    IObjectService* pObjMgr = m_pGameInstance->Get_ObjectMgr();

    CGameObject* pStaticObject = Builder::Create_Object({ "MapTool_Level" ,"Proto_GameObject_StaticObject" })
    	.Position(pRayHit->vHittedPosition)
    	.Scale(m_vScale_PlacedObject)
    	.Build("Static_Model");
    
    pObjMgr->Add_Object(pStaticObject,	{ "MapTool_Level", "Static_Layer" });


}

CMapToolGui* CMapToolGui::Create(GUI_CONTEXT* pContext)
{
	CMapToolGui* pInstance = new CMapToolGui(pContext);
	if (FAILED(pInstance->Initialize())) {
		MSG_BOX("MapToolGui Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapToolGui::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
