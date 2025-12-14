#include "AIChannel.h"

CAIChannel::CAIChannel()
{
}

HRESULT CAIChannel::Initialize(const aiNodeAnim* pAIChannel, CAIModelData* pAIModelData)
{
	return E_NOTIMPL;
}

CAIChannel* CAIChannel::Create(const aiNodeAnim* pAIChannel, CAIModelData* pAIModelData)
{
    CAIChannel* pInstance = new CAIChannel();

    if (FAILED(pInstance->Initialize(pAIChannel, pAIModelData))) {
        MSG_BOX("Create Failed : Engine | CAIChannel");
        return nullptr;
    }

    return pInstance;
}

void CAIChannel::Free()
{
	__super::Free();
}
