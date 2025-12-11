#pragma once
#include "IService.h"
NS_BEGIN(Engine)
class ENGINE_DLL IUI_Service abstract :
	public IService
{
protected:
    virtual  ~IUI_Service() DEFAULT;
public:
	virtual void Pre_EngineUpdate(_float dt) PURE;
	virtual void Post_EngineUpdate(_float dt) PURE;
	virtual void Priority_Update(_float dt) PURE;
	virtual void Update(_float dt) PURE;
	virtual void Late_Update(_float dt) PURE;
public:
	virtual void Clear(const string& LevelTag) PURE;
	virtual HRESULT Sync_To_Level() PURE;
	virtual HRESULT Add_UIObject(class CUI_Object* object, const string& level) PURE;
	virtual void Remove_UIObject(class CUI_Object* object)PURE;
};
NS_END
