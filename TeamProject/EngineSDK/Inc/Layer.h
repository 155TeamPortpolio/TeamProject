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
private:
	vector<class CGameObject*>			m_GameObjects; /*순회용*/
	unordered_map<_uint, _uint>				m_IndexByID; /*오브젝트 ID / 벡터 인덱스*/
	/*오브젝트 아이디, 해당 레이어 상의 인덱스*/

	_bool m_isRender = { true };
public:
	static CLayer* Create();
	virtual void Free() override;
};

NS_END