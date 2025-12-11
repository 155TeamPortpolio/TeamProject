#pragma once
#include "IProtoService.h"

NS_BEGIN(Engine)

using PROTOTYPES = unordered_map<string,  CGameObject*>;

class CPrototypeMgr final: public IProtoService
{
private:
	CPrototypeMgr();
	virtual ~CPrototypeMgr() override DEFAULT;
	HRESULT Initialize();

public:
	HRESULT Sync_To_Level();
	HRESULT Add_ProtoType(const string& LevelTag, const string& protoTag, CGameObject* pProto);
	 CGameObject* Clone_Prototype(const string& LevelTag, const string& protoTag, INIT_DESC* pArg);
	void Clear(const string& LevelTag);

private:
	 CGameObject* Find_Prototype(const string& LevelTag, const string& protoTag);

private:
	unordered_map<string,PROTOTYPES> m_Prototypes; //<레벨 / 프로토타입(키/원형)>

public:
	static CPrototypeMgr* Create();
	virtual void Free() override;
};
NS_END
