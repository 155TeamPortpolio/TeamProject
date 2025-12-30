#include "Engine_Defines.h"
#include "GameInstance.h"
#include "Sprite2D.h"
#include "Shader.h"
#include "Texture.h"
#include "IResourceService.h"
#include "VI_Point.h"
#include "UI_Object.h"

namespace
{
	struct AlphaCache
	{
		_uint width{};
		_uint height{};
		vector<_ubyte> alpha;
		vector<_ubyte> outside;
	};

	unordered_map<ID3D11ShaderResourceView*, AlphaCache> g_alphaCache;

	AlphaCache& GetAlphaCache(ID3D11ShaderResourceView* srv)
	{
		auto it = g_alphaCache.find(srv);
		if (it != g_alphaCache.end()) return it->second;

		AlphaCache cache{};

		ID3D11Resource* res{};
		srv->GetResource(&res);

		ID3D11Texture2D* srcTex{};
		HRESULT hr = res->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&srcTex);
		Safe_Release(res);

		if (FAILED(hr) || !srcTex)
		{
			auto r = g_alphaCache.emplace(srv, AlphaCache{});
			return r.first->second;
		}

		D3D11_TEXTURE2D_DESC desc{};
		srcTex->GetDesc(&desc);

		cache.width = desc.Width;
		cache.height = desc.Height;
		cache.alpha.resize((size_t)cache.width * (size_t)cache.height);

		D3D11_TEXTURE2D_DESC stagingDesc = desc;
		stagingDesc.Usage = D3D11_USAGE_STAGING;
		stagingDesc.BindFlags = 0;
		stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		stagingDesc.MiscFlags = 0;

		ID3D11Device* dev{};
		srcTex->GetDevice(&dev);

		ID3D11Texture2D* stagingTex{};
		hr = dev->CreateTexture2D(&stagingDesc, nullptr, &stagingTex);
		if (FAILED(hr) || !stagingTex)
		{
			Safe_Release(dev);
			Safe_Release(srcTex);

			auto r = g_alphaCache.emplace(srv, AlphaCache{});
			return r.first->second;
		}

		auto ctx = CGameInstance::GetInstance()->Get_Context();
		ctx->CopyResource(stagingTex, srcTex);

		D3D11_MAPPED_SUBRESOURCE mapped{};
		hr = ctx->Map(stagingTex, 0, D3D11_MAP_READ, 0, &mapped);
		if (SUCCEEDED(hr))
		{
			for (_uint y = 0; y < cache.height; ++y)
			{
				_ubyte* row = (_ubyte*)mapped.pData + (size_t)mapped.RowPitch * y;
				for (_uint x = 0; x < cache.width; ++x)
					cache.alpha[(size_t)y * cache.width + x] = row[x * 4 + 3];
			}

			ctx->Unmap(stagingTex, 0);
		}

		Safe_Release(stagingTex);
		Safe_Release(dev);
		Safe_Release(srcTex);

		cache.outside.assign((size_t)cache.width * (size_t)cache.height, 0);

		vector<size_t> stack;
		stack.reserve((size_t)cache.width + (size_t)cache.height);

		auto push_if_outside = [&](int x, int y)
			{
				if (x < 0 || y < 0) return;
				if (x >= (int)cache.width || y >= (int)cache.height) return;

				size_t idx = (size_t)y * cache.width + (size_t)x;
				if (cache.outside[idx]) return;
				if (cache.alpha[idx] != 0) return;

				cache.outside[idx] = 1;
				stack.push_back(idx);
			};

		for (int x = 0; x < (int)cache.width; ++x)
		{
			push_if_outside(x, 0);
			push_if_outside(x, (int)cache.height - 1);
		}

		for (int y = 0; y < (int)cache.height; ++y)
		{
			push_if_outside(0, y);
			push_if_outside((int)cache.width - 1, y);
		}

		while (!stack.empty())
		{
			size_t idx = stack.back();
			stack.pop_back();

			int x = (int)(idx % cache.width);
			int y = (int)(idx / cache.width);

			push_if_outside(x - 1, y);
			push_if_outside(x + 1, y);
			push_if_outside(x, y - 1);
			push_if_outside(x, y + 1);
		}

		auto r = g_alphaCache.emplace(srv, move(cache));
		return r.first->second;
	}

	_bool HitTestAlphaCache(const AlphaCache& cache, _float u, _float v, _float alphaThreshold)
	{
		if (cache.width == 0 || cache.height == 0) return true;

		if (u < 0.f) u = 0.f;
		if (v < 0.f) v = 0.f;
		if (u > 1.f) u = 1.f;
		if (v > 1.f) v = 1.f;

		_uint x = (_uint)(u * (cache.width - 1));
		_uint y = (_uint)(v * (cache.height - 1));

		size_t idx = (size_t)y * cache.width + x;

		_ubyte a8 = cache.alpha[idx];
		_float a = (_float)a8 / 255.f;

		if (a > alphaThreshold) return true;

		if (a8 == 0 && cache.outside[idx] == 0) return true;

		return false;
	}
}


_bool CSprite2D::HitTest_AlphaUV(_float u, _float v, _float alphaThreshold)
{
	if (m_pTextures.empty()) return true;

	auto tex = m_pTextures[m_iDrawIndex];
	if (!tex) return true;

	auto srv = tex->Get_SRV();
	if (!srv) return true;

	const AlphaCache& cache = GetAlphaCache(srv);
	return HitTestAlphaCache(cache, u, v, alphaThreshold);
}
// ------------------------------------------
CSprite2D::CSprite2D(const CSprite2D& rhs)
	:CComponent(rhs),
	m_pShader(rhs.m_pShader),
	m_pPoint(rhs.m_pPoint),
	m_pTextures(rhs.m_pTextures),
	m_TextKey(rhs.m_TextKey)
{
	Safe_AddRef(m_pShader);
	Safe_AddRef(m_pPoint);
	for (auto tex : m_pTextures)
		Safe_AddRef(tex);
}

HRESULT CSprite2D::Initialize_Prototype()
{
	m_pPoint = CVI_Point::Create(CGameInstance::GetInstance()->Get_Device(), "Sprite2D");

	return S_OK;
}

HRESULT CSprite2D::Initialize(COMPONENT_DESC* pArg)
{
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

	for (auto& Slot : m_DynamicSlots) 
		m_pShader->Bind_Value(Slot.first, Slot.second);


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
	if (Index >= m_pTextures.size()) 
		return E_FAIL;
	

	if(m_pTextures[Index] == nullptr)
		return E_FAIL;

	m_iDrawIndex = Index;
	return S_OK;
}

CVIBuffer* CSprite2D::Get_Buffer()
{
	return m_pPoint;
}

HRESULT CSprite2D::Add_Texture(const string& levelKey, const string& texKey)
{
	CTexture* pTexture = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_Texture(levelKey, texKey, true);

	if (!pTexture)
		return E_FAIL;
	m_pTextures.push_back(pTexture);

	Safe_AddRef(pTexture);
	return S_OK;
}

HRESULT CSprite2D::Change_Texture(_uint idx, const string& levelKey, const string& texKey)
{
	if (idx >= m_pTextures.size())
		Add_Texture(levelKey, texKey);
	else
	{
		if (m_pTextures[idx])
			Safe_Release(m_pTextures[idx]);

		auto tex = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_Texture(levelKey, texKey, true);
		if (!tex)
			return E_FAIL;

		m_pTextures[idx] = tex;
		Safe_AddRef(tex);
	}
	return S_OK;
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

HRESULT CSprite2D::Set_TextKey(const string& textKey)
{
	m_TextKey = textKey;
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
	if (!m_pTextures.empty()) 
	{
		ImGui::Image((ImTextureID)m_pTextures.front()->Get_SRV(),
			ImVec2(1280 / 5, 720 / 5));
	}
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

void CSprite2D::Free()
{
	__super::Free();
	Safe_Release(m_pPoint);
	Safe_Release(m_pShader);

	for (auto& texture : m_pTextures)
		Safe_Release(texture);
}