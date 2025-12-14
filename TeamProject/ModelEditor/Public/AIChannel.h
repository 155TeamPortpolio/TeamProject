#pragma once
#include "ModelEditor_Defines.h"
#include "Channel.h"
#include "Assimps.h"

NS_BEGIN(ModelEdit)
class CAIChannel :
	public CChannel
{
private:
	CAIChannel();
	virtual ~CAIChannel() DEFAULT;

public:
	virtual HRESULT Initialize(const aiNodeAnim* pAIChannel, class CAIModelData* pAIModelData);
	void Save_File(ofstream& ofs);

public:
	static CAIChannel* Create(const aiNodeAnim* pAIChannel, class CAIModelData* pAIModelData);
	virtual void Free() override;
};

NS_END