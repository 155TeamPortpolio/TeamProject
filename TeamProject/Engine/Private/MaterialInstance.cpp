#include "Engine_Defines.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "Shader.h"

CMaterialInstance::CMaterialInstance(CMaterialData* pData, ID3D11Device* pDevice)
	: m_pMaterialData{ pData }, m_pDevice{ pDevice }
{
	Safe_AddRef(m_pMaterialData);
	Safe_AddRef(m_pDevice);
}

CMaterialInstance::CMaterialInstance(const CMaterialInstance& rhs)
	:m_pDevice{ rhs.m_pDevice }
	, m_pMaterialData{ rhs.m_pMaterialData }
	, m_TextureIndexs{ rhs.m_TextureIndexs }
	, overrides_Constant{ rhs.overrides_Constant }
{
	Safe_AddRef(m_pMaterialData);
	Safe_AddRef(m_pDevice);
}

void CMaterialInstance::ApplyData(ID3D11DeviceContext* pContext)
{
	/*상수 버퍼*/

	if (nullptr != m_pCBuffer) {
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		pContext->Map(m_pCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, &overrides_Constant, sizeof(MaterialConstants));
		pContext->Unmap(m_pCBuffer, 0);
		m_pMaterialData->Set_MaterialConstantBuffer(m_pCBuffer);
	}

	m_pMaterialData->ApplyData(pContext, m_TextureIndexs);
	CShader* pMaterialShader = m_pMaterialData->Get_Shader();

	for (auto& Slot : m_DynamicSlots) {
		pMaterialShader->Bind_Value(Slot.first, Slot.second);
	}

	pMaterialShader->Apply(Get_PassConstant(), pContext);
}

const string& CMaterialInstance::Get_PassConstant()
{
	if (override_Pass.empty())
		return m_pMaterialData->Get_PassConstant();
	else
		return override_Pass;
}


HRESULT CMaterialInstance::Create_CBuffer(ID3D11Device* pDevice)
{
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = sizeof(MaterialConstants);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pDevice->CreateBuffer(&desc, nullptr, &m_pCBuffer);
	m_pCBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, strlen("MaterialCBuffer"), "MaterialCBuffer");

	return S_OK;
}

CShader* CMaterialInstance::Get_Shader()
{
	return m_pMaterialData->Get_Shader();
}

_uint CMaterialInstance::Get_ShaderID()
{
	return m_pMaterialData->Get_ShaderID();
}

_uint CMaterialInstance::Get_MaterialDataID()
{
	return m_pMaterialData->Get_MaterialDataID();
}

const string& CMaterialInstance::Get_MaterialName()
{
	return m_pMaterialData->Get_MaterialName();
}

HRESULT CMaterialInstance::Set_Param(const string& ConstantName, const SHADER_PARAM& parameter)
{
	SHADER_PARAM DynamicSlot = parameter;
	auto iter = m_DynamicSlots.emplace(ConstantName, DynamicSlot);

	if (false == iter.second)
		return E_FAIL;

	else
		return S_OK;
}

SHADER_PARAM* CMaterialInstance::Get_Param(const string& ConstantName)
{
	auto iter = m_DynamicSlots.find(ConstantName);

	if (iter != m_DynamicSlots.end()) {
		return &(iter->second);
	}

	return nullptr;
}

HRESULT CMaterialInstance::Override_Constant(const MaterialConstants& materialConstant)
{
	overrides_Constant = materialConstant;
	return S_OK;
}

HRESULT CMaterialInstance::Reset_Constant()
{
	overrides_Constant = m_pMaterialData->Get_DefaultMaterialConstant();
	return S_OK;
}

void CMaterialInstance::ChangeTexture(TEXTURE_TYPE type, _uint index)
{
	m_TextureIndexs[static_cast<_uint>(type)]= index;
}

HRESULT CMaterialInstance::Reset_Pass()
{
	override_Pass = m_pMaterialData->Get_PassConstant();
	return S_OK;
}

void CMaterialInstance::Render_GUI()
{
	m_pMaterialData->Render_GUI(m_TextureIndexs);
}

CMaterialInstance* CMaterialInstance::Make_Handle(CMaterialData* pData, ID3D11Device* pDevice)
{
	CMaterialInstance* hMaterial = new CMaterialInstance(pData, pDevice);

	hMaterial->overrides_Constant = pData->Get_DefaultMaterialConstant();
	hMaterial->m_TextureIndexs.resize(MAX_TEXTURE_TYPE_VALUE, 0);

	if (FAILED(hMaterial->Create_CBuffer(hMaterial->m_pDevice))) {
		Safe_Release(hMaterial);
	};
	return hMaterial;
}

CMaterialInstance* CMaterialInstance::Create_Handle(const string& materialKey, const string& DefualtpassConstant, ID3D11Device* pDevice)
{
	CMaterialData* pData = CMaterialData::Create(materialKey, DefualtpassConstant);
	CMaterialInstance* hMaterial = new CMaterialInstance(pData, pDevice);

	hMaterial->overrides_Constant = pData->Get_DefaultMaterialConstant();
	hMaterial->m_TextureIndexs.resize(MAX_TEXTURE_TYPE_VALUE, 0);
	hMaterial->override_Pass = DefualtpassConstant;

	if (FAILED(hMaterial->Create_CBuffer(hMaterial->m_pDevice))) {
		Safe_Release(hMaterial);
	};

	/*직접 생성해주었으니, 안에 넣고 나면 addRef되고,
	그거 레퍼런스 카운트 하나 다운 시켜주어야 함*/
	Safe_Release(pData);

	return hMaterial;
}

CMaterialInstance* CMaterialInstance::Clone()
{
	CMaterialInstance* hMaterial = new CMaterialInstance(*this);

	if (FAILED(hMaterial->Create_CBuffer(hMaterial->m_pDevice))) {
		Safe_Release(hMaterial);
	};

	return hMaterial;
}

void CMaterialInstance::Free()
{
	Safe_Release(m_pMaterialData);
	Safe_Release(m_pCBuffer);
	Safe_Release(m_pDevice);
}