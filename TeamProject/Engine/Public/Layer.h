#pragma once
#include "Base.h"
NS_BEGIN(Engine)

class ENGINE_DLL CLayer final :
    public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Pre_EngineUpdate(_float dt);
	void Post_EngineUpdate(_float dt);
	void Priority_Update(_float dt);
	void Update(_float dt);
	void Late_Update(_float dt);
	class CGameObject* Pop_GameObject(_uint ObjectID);
	void Remove_GameObject(_uint ObjectID);
	_uint Get_ObjectCount() { return m_IndexByID.size(); };
	class CGameObject* Find_ObjectByID(_uint ObjectID);
	const vector<class CGameObject*>& Get_AllObject() { return m_GameObjects; };

public:
	void Set_RenderState(_bool render) { m_isRender = render; };
	string Get_LayerTag() { return m_LayerTag; }

public:
	_bool isLayerAcitve() { return m_isRenderActive; }
	void Set_LayerActive(_bool active) {m_isRenderActive= active; }

public: 
	void Clear_Layer();
	void Set_TimeScale(_float scale) { m_LayerTimeScale = scale; };
	void Reset_TimeScale() { m_LayerTimeScale = 1.f; };
	_float Get_TimeScale() { return m_LayerTimeScale; }

private:
	vector<class CGameObject*>			m_GameObjects; /*순회용*/
	unordered_map<_uint, _uint>				m_IndexByID; /*오브젝트 ID / 벡터 인덱스*/
	/*오브젝트 아이디, 해당 레이어 상의 인덱스*/

	_bool m_isRender = { true };
	_bool m_isRenderActive = { true };
	string m_LayerTag = { true };
	_float m_LayerTimeScale = { 1.f };

public:
	static CLayer* Create();
	static CLayer* Create(string layerTag);
	virtual void Free() override;
};

NS_END