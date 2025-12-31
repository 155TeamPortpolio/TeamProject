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

private:
	CClickManager(HWND hWnd) : m_hWnd(hWnd) {}
	virtual ~CClickManager() {}

public:
	void Update(_float dt);
	void Add_ClickableObject(CUI_Object* object);

private:
	const AlphaCache& GetOrBuildAlphaCache(const string& key, ID3D11ShaderResourceView* srv);
	AlphaCache        BuildAlphaCache(ID3D11ShaderResourceView* srv);
	_bool             HitTestAlphaCache(const AlphaCache& cache, _float u, _float v, _float alphaThreshold);

private:
	HWND				m_hWnd{};
	vector<CUI_Object*>	m_clickableObjs{};
	CUI_Object*         m_pHovered{};
	CUI_Object*         m_pNewHovered{};

	unordered_map<string, AlphaCache> m_alphaCache{};

public:
	static CClickManager* Create(HWND hWnd) { return new CClickManager(hWnd); }
	virtual void Free() override;
};

NS_END