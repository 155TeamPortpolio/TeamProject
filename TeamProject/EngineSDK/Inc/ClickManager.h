#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CUI_Object;

class CClickManager : public CBase
{
private:
	struct AlphaCache
	{
		_uint width{};
		_uint height{};
		vector<_ubyte> alpha;
		vector<_ubyte> outside;
	}; 
	
	enum class UISlotState {
		NONE = 0,           // 비어 있음 (슬롯 미사용)
		ACTIVE = 1,         // 사용 가능
		DEAD = 2,           // 소유자가 삭제됨
	};

	struct UISlot {
		class CUI_Object* pUI = { nullptr };
		UISlotState eState = { UISlotState::NONE };
	};

private:
	CClickManager(HWND hWnd) : m_hWnd(hWnd) {}
	virtual ~CClickManager() {}

public:
	void Update(_float dt);
	void Register_ClickableObject(CUI_Object* pObject);
	void Unregister_ClickableObject(CUI_Object* pObject);

private:
	const AlphaCache& GetOrBuildAlphaCache(const string& key, ID3D11ShaderResourceView* srv);
	AlphaCache        BuildAlphaCache(ID3D11ShaderResourceView* srv);
	_bool             HitTestAlphaCache(const AlphaCache& cache, _float u, _float v, _float alphaThreshold);

private:
	HWND				m_hWnd{};
	vector<UISlot>		m_clickableSlots;
	UISlot				m_hoveredSlot = {};

	unordered_map<string, AlphaCache> m_alphaCache{};

public:
	static CClickManager* Create(HWND hWnd) { return new CClickManager(hWnd); }
	virtual void Free() override;
};

NS_END