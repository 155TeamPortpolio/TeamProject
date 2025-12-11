#pragma once
#include "IService.h"
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL IProtoService abstract:
    public IService
{
protected:
    virtual ~IProtoService() override DEFAULT;

public:
	virtual HRESULT Sync_To_Level() PURE;
	virtual HRESULT Add_ProtoType(const string& LevelTag, const string& protoTag, CGameObject* pProto) PURE;
	virtual CGameObject* Clone_Prototype(const string& LevelTag, const string& protoTag, INIT_DESC* pArg) PURE;
	virtual void Clear(const string& LevelTag) PURE;
};

NS_END
