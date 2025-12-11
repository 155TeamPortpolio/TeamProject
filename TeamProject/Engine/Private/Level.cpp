#include "Engine_Defines.h"
#include "Level.h"
#include "GameInstance.h"
#include "ILevelService.h"
#include "LevelObject.h"

CLevel::CLevel(const string& LevelKey)
	:m_LevelKey{ LevelKey }
{
}

HRESULT CLevel::Awake()
{
	return S_OK;
}

HRESULT CLevel::Render(ID3D11DeviceContext* pContext)
{
	return S_OK;
}

void CLevel::Free()
{
	__super::Free();

	for (auto& pair : m_pLevelObjects) {
		Safe_Release(pair.second);
	}

}
