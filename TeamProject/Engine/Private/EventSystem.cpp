#include "Engine_Defines.h"
#include "EventSystem.h"

CEventSystem::CEventSystem()
{
}

HRESULT CEventSystem::Initialize() {

}

CEventSystem* CEventSystem::Create()
{
    CEventSystem* pInstance = new CEventSystem();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Create : CEventSystem");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CEventSystem::Free()
{
    for (auto& [index, typeSafe] : m_Callbacks)
        delete typeSafe;

    m_Callbacks.clear();

    __super::Free();
}
