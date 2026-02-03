#pragma once
#include "Stage.h"

NS_BEGIN(Engine)
class CGameInstance;
class CUI_Object;
class IProtoService;
class IObjectService;
NS_END

NS_BEGIN(Client)
class CZeroStage_Boss :
    public CStage
{
private:
	CZeroStage_Boss();
	~CZeroStage_Boss() DEFAULT;

public:
	virtual HRESULT Initialize(class CZero_Level* pOwnerLevel);
	virtual HRESULT Awake() override;
	virtual void    Update()override;

public:
	virtual HRESULT Enter_Stage(CZero_Level::StageContext& context)override;

private:
	void Intro();
	void Battle();
	void Outro();
	void End();

private:
	_bool m_isSequenceEnd = {};
public:
	static CZeroStage_Boss* Create(class CZero_Level* pOwnerLevel);
	virtual void Free();
};

NS_END