#include "Engine_Defines.h"
#include "GameInstance.h"
#include "Sprite2D.h"
#include "Shader.h"
#include "Texture.h"
#include "IResourceService.h"
#include "VI_Point.h"

CSprite2D::CSprite2D()
{
}

CSprite2D::CSprite2D(const CSprite2D& rhs)
	:CComponent(rhs)
{
}

CSprite2D::~CSprite2D()
{
}

HRESULT CSprite2D::Initialize_Prototype()
{
	
	return S_OK;
}


HRESULT CSprite2D::Initialize(COMPONENT_DESC* pArg)
{
	m_pPoint = CVI_Point::Create(CGameInstance::GetInstance()->Get_Device(), "Sprite2D");
	if (m_pPoint == nullptr)
		return E_FAIL;

	return S_OK;
}

void CSprite2D::Apply_Shader(ID3D11DeviceContext* pContext)
{
	if (m_pShader == nullptr) return;

	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };

	for (UINT slot = 0; slot < MAX_TEXTURE_TYPE_VALUE; ++slot)
		pContext->PSSetShaderResources(slot, 1, nullSRV);

	if (!m_pTextures.empty() && m_pTextures[m_iDrawIndex] != nullptr) {
		SHADER_PARAM param = {};
		param.typeName = "Texture2D";
		param.iSize = 0;
		param.pData = m_pTextures[m_iDrawIndex]->Get_SRV();

		m_pShader->Bind_Value("SpriteTexture", param);
	}

	for (auto& Slot : m_DynamicSlots) {
		m_pShader->Bind_Value(Slot.first, Slot.second);
	}

	m_pShader->Apply(m_PassConstant, pContext);
}

void CSprite2D::Draw_Sprite(ID3D11DeviceContext* pContext)
{
	if (m_pPoint)
	{
		m_pPoint->Bind_Buffer(pContext);
		m_pPoint->Render(pContext);
	}
}

HRESULT CSprite2D::ChangeSprite(_uint Index)
{
	if (Index >= m_pTextures.size()) {
		return E_FAIL;
	}

	if(m_pTextures[Index] == nullptr)
		return E_FAIL;

	m_iDrawIndex = Index;
	return S_OK;
}

CVIBuffer* CSprite2D::Get_Buffer()
{
	return m_pPoint;
}

HRESULT CSprite2D::Add_Texture(const string& levelKey, const string& TextureKey)
{
	CTexture* pTexture = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_Texture(levelKey, TextureKey);

	if (!pTexture)
		return E_FAIL;
	m_pTextures.push_back(pTexture);

	Safe_AddRef(pTexture);
	return S_OK;
}

HRESULT CSprite2D::Change_Texture(_uint index, const string& levelKey, const string& TextureKey)
{
	if (index >= m_pTextures.size()){
		Add_Texture(levelKey, TextureKey);
		return S_OK;
	}
	else {
		if (m_pTextures[index])
			Safe_Release(m_pTextures[index]);

		CTexture* pTexture = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_Texture(levelKey, TextureKey);

		if (!pTexture)
			return E_FAIL;
		m_pTextures[index] = pTexture;
		Safe_AddRef(pTexture);
		return S_OK;
	}
}

HRESULT CSprite2D::Link_Shader(const string& levelKey, const string& shaderKey)
{
	Safe_Release(m_pShader);
	m_pShader = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_Shader(levelKey, shaderKey);

	if (!m_pShader)
		return E_FAIL;

	Safe_AddRef(m_pShader);
	return S_OK;
}

HRESULT CSprite2D::ChangePass(const string& passConstant)
{
	if (passConstant.empty())
		return S_OK;

	m_PassConstant = passConstant;
	return S_OK;
}

HRESULT CSprite2D::Set_Param(const string& ConstantName, const SHADER_PARAM& parameter)
{
	auto it = m_DynamicSlots.find(ConstantName);
	if (it != m_DynamicSlots.end())
	{
		it->second = parameter;    // 기존 값 덮어쓰기
		return S_OK;
	}
	SHADER_PARAM newparameter = parameter;
	m_DynamicSlots.emplace(ConstantName, newparameter);
	return S_OK;
}

SHADER_PARAM* CSprite2D::Get_Param(const string& ConstantName)
{
	auto iter = m_DynamicSlots.find(ConstantName);

	if (iter != m_DynamicSlots.end()) {
		return &(iter->second);
	}

	return nullptr;
}

bool CSprite2D::IsValid()
{
	if (m_pPoint == nullptr || m_pShader == nullptr)
		return false;

	if (!m_pTextures.empty())
		return true;

	auto it = m_DynamicSlots.find("SpriteTexture");
	if (it != m_DynamicSlots.end())
		return true;

	return false;
}


void CSprite2D::Render_GUI()
{
}

CSprite2D* CSprite2D::Create()
{
	CSprite2D* instance = new CSprite2D();
	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Sprite2D Comp Failed To Create : CSprite2D");
	}
	return instance;
}

CComponent* CSprite2D::Clone()
{
	return new CSprite2D(*this);
}

void CSprite2D::Free()
{
	__super::Free();
	Safe_Release(m_pPoint);
	Safe_Release(m_pShader);

	for (auto& texture : m_pTextures)
	{
		Safe_Release(texture);
	}
}
