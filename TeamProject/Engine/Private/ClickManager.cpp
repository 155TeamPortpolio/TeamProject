#include "Engine_Defines.h"
#include "ClickManager.h"
#include "GameInstance.h"
#include "UI_Object.h"
#include "Sprite2D.h"
#include "Texture.h"

void CClickManager::Update(_float dt)
{
	if (m_clickableSlots.empty()) return;

	auto pInput = CGameInstance::GetInstance()->Get_InputDev();
	Vector2 mousePos = pInput->Mouse_Pos();

    UISlot m_newHoveredSlot = {};

	while (!m_clickableSlots.empty())
	{
        auto& slot = m_clickableSlots.back();

		const auto pObj = slot.pUI;
        m_clickableSlots.pop_back();

		Vector2 topLeft = pObj->Get_RectTopLeft_Screen();
		Vector2 sizePx  = pObj->Get_PxSize();

		if (mousePos.x < topLeft.x)            continue;
		if (mousePos.y < topLeft.y)            continue;
		if (mousePos.x > topLeft.x + sizePx.x) continue;
		if (mousePos.y > topLeft.y + sizePx.y) continue;

		Vector2 local = mousePos - topLeft;
		_float  u     = local.x / sizePx.x;
		_float  v     = local.y / sizePx.y;
        
        static constexpr _float alphaThreshold = 0.25f;

        if (alphaThreshold > 0.f)
        {
            auto pSprite       = pObj->Get_Component<CSprite2D>();
            if (!pSprite->IsValid()) continue;
            auto pTex          = pSprite->Get_Texture(0);
            if (!pTex) continue;
            const string& key = pTex->Get_Key();

            auto srv = pTex->Get_SRV();
            const AlphaCache& cache = GetOrBuildAlphaCache(key, srv);

            if (!HitTestAlphaCache(cache, u, v, alphaThreshold)) continue;
        }

        m_newHoveredSlot = slot;

		if (pInput->Mouse_Tap(MOUSE_BTN::LB))
			pObj->OnClick();

		break;
	}

	if (m_hoveredSlot.pUI != m_newHoveredSlot.pUI)
	{
		if (m_hoveredSlot.eState == UISlotState::ACTIVE)
            m_hoveredSlot.pUI->Exit_Hover();

        m_hoveredSlot = m_newHoveredSlot;

        if (m_hoveredSlot.eState == UISlotState::ACTIVE)
            m_hoveredSlot.pUI->Enter_Hover();
	}

    m_clickableSlots.clear();
}

void CClickManager::Register_ClickableObject(CUI_Object* pObject)
{
    UISlot slot = {};

    slot.pUI = pObject;
    slot.eState = UISlotState::ACTIVE;
    m_clickableSlots.push_back(slot);
}

void CClickManager::Unregister_ClickableObject(CUI_Object* pObject)
{
    if (m_hoveredSlot.pUI == pObject)
    {
        m_hoveredSlot.pUI = nullptr;
        m_hoveredSlot.eState = UISlotState::DEAD;
    }
}

const CClickManager::AlphaCache& CClickManager::GetOrBuildAlphaCache(const string& key, ID3D11ShaderResourceView* srv)
{
	auto it = m_alphaCache.find(key);
	if (it != m_alphaCache.end()) return it->second;

	AlphaCache cache = BuildAlphaCache(srv);
	auto r = m_alphaCache.emplace(key, move(cache));
	return r.first->second;
}

CClickManager::AlphaCache CClickManager::BuildAlphaCache(ID3D11ShaderResourceView* srv)
{
    // 1. srcTex가 CPU에서 읽을 수 있는 DXGI_FORMAT_R8G8B8A8_UNORM, B8G8R8A8_UNORM 같은 32bpp 포맷이어여함.
    AlphaCache cache{};

    ID3D11Resource* res{};
    srv->GetResource(&res);

    ID3D11Texture2D* srcTex{};
    HRESULT hr = res->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&srcTex);
    Safe_Release(res);

    if (FAILED(hr) || !srcTex) return cache;

    D3D11_TEXTURE2D_DESC desc{};
    srcTex->GetDesc(&desc);

    cache.width = desc.Width;
    cache.height = desc.Height;
    cache.alpha.resize((size_t)cache.width * (size_t)cache.height);

    D3D11_TEXTURE2D_DESC stagingDesc = desc;
    stagingDesc.Usage          = D3D11_USAGE_STAGING;
    stagingDesc.BindFlags      = 0;
    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    stagingDesc.MiscFlags      = 0;

    ID3D11Device* dev{};
    srcTex->GetDevice(&dev);

    ID3D11Texture2D* stagingTex{};
    hr = dev->CreateTexture2D(&stagingDesc, nullptr, &stagingTex);
    if (FAILED(hr) || !stagingTex)
    {
        Safe_Release(dev);
        Safe_Release(srcTex);
        return AlphaCache{};
    }

    auto context = CGameInstance::GetInstance()->Get_Context();
    context->CopyResource(stagingTex, srcTex);

    D3D11_MAPPED_SUBRESOURCE mapped{};
    hr = context->Map(stagingTex, 0, D3D11_MAP_READ, 0, &mapped);
    if (SUCCEEDED(hr))
    {
        for (_uint y = 0; y < cache.height; ++y)
        {
            _ubyte* row = (_ubyte*)mapped.pData + (size_t)mapped.RowPitch * y;
            for (_uint x = 0; x < cache.width; ++x)
                cache.alpha[(size_t)y * cache.width + x] = row[x * 4 + 3];
        }
        context->Unmap(stagingTex, 0);
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

    return cache;
}

_bool CClickManager::HitTestAlphaCache(const AlphaCache& cache, _float u, _float v, _float alphaThreshold)
{
    if (cache.width == 0 || cache.height == 0) return true;

    u = clamp(u, 0.f, 1.f);
    v = clamp(v, 0.f, 1.f);

    _uint x = (_uint)(u * (_float)(cache.width - 1));
    _uint y = (_uint)(v * (_float)(cache.height - 1));

    x = clamp(x, (_uint)0, cache.width - 1);
    y = clamp(y, (_uint)0, cache.height - 1);

    size_t idx = (size_t)y * cache.width + x;

    _ubyte a8 = cache.alpha[idx];
    _float a  = (_float)a8 / 255.f;

    if (a > alphaThreshold) return true;
    if (a8 == 0 && cache.outside[idx] == 0) return true;

    return false;
}

void CClickManager::Free()
{
	__super::Free();
    m_clickableSlots.clear();
    m_alphaCache.clear();
}