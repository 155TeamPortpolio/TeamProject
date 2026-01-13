#pragma once
#include "IStage.h"
NS_BEGIN(Client)
class CZeroStage_Boss :
    public IStage
{
private:
	CZeroStage_Boss();
	~CZeroStage_Boss() DEFAULT;

public:
	virtual HRESULT Initialize(class CZero_Level* pOwnerLevel);
	virtual void    Update()override;
	virtual HRESULT Render()override;

public:
	virtual HRESULT Ready_Stage()override;
	virtual HRESULT Enter_Stage()override;
	virtual HRESULT Exit_Stage()override;

private:
	class CZero_Level* m_pOwnerLevel = { nullptr };

public:
	static CZeroStage_Boss* Create(class CZero_Level* pOwnerLevel);
	virtual void Free();
};

NS_END