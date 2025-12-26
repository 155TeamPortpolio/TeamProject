#include "Engine_Defines.h"
#include "ResourceThread.h"
#include "GameInstance.h"
#include "Helper_Func.h"

#include "VIBuffer.h"
#include "VI_Rect.h"
#include "VI_Cube.h"
#include "VI_Terrain.h"
#include "VI_Plane.h"
#include "VI_Point.h"
#include "VI_InstancePoint.h"

#include "Shader.h"
#include "Material.h"
#include "Texture.h"
#include "SoundData.h"
#include "MaterialData.h"
#include "ModelData.h"
#include "MaterialInstance.h"
#include "AnimationClip.h"
#include "AnimationLayout.h"
#include "ComputeShader.h"

CResourceThread::CResourceThread(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }, m_pContext{ pContext }, m_pInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pInstance);
}

HRESULT CResourceThread::Initiallize()
{
	return S_OK;
}


void CResourceThread::Clear_Resource(const string& levelTag)
{
	
}

HRESULT CResourceThread::Sync_To_Level()
{
	
	return S_OK;
}

CSoundData* CResourceThread::Load_Sound(const string& levelTag, const string& soundKey)
{
	
}

CVIBuffer* CResourceThread::Load_VIBuffer(const string& levelTag, const string& bufferKey, BUFFER_TYPE eType)
{
}

vector<CMaterialInstance*> CResourceThread::Load_MaterialFromFile(
	const string& levelTag,
	const string& fileKey)
{

}

CShader* CResourceThread::Load_Shader(const string& levelTag, const string& shaderKey)
{
}

CTexture* CResourceThread::Load_Texture(const string& levelTag, const string& textureKey, _bool sRGBType)
{
}

vector<CAnimationClip*> CResourceThread::Load_MetaClip(const string& levelTag, const string& MetaClipKey)
{
}

EFFECT_ASSET CResourceThread::Load_EffectAsset(const string& levelTag, const string& effectTag)
{
}

CComputeShader* CResourceThread::Load_ComputeShader(const string& levelTag, const string& shaderKey)
{
}

CModelData* CResourceThread::Load_ModelData(const string& levelTag, const string& ModelKey)
{
}

HRESULT CResourceThread::Add_ResourcePath(const string& resourceKey, const string& resourcePath)
{
}

_int CResourceThread::ValidLevel(const string& levelKey)
{;
}

void CResourceThread::Load_InitialResource()
{
}


string CResourceThread::MakePath(const string& pathKey)
{
}


CResourceThread* CResourceThread::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CResourceThread* instance = new CResourceThread(pDevice, pContext);
	if (FAILED(instance->Initiallize())) {
		Safe_Release(instance);
	}
	return instance;
}

void CResourceThread::Free()
{
	__super::Free();

}
