#pragma once
#include "ModelEditor_Defines.h"
#include "Assimps.h"
#include "Base.h"

NS_BEGIN(ModelEdit)

class CAIMaterial final : public CBase
{
private:
	CAIMaterial(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CAIMaterial() = default;


private:
	ID3D11Device*							m_pDevice = { nullptr };
	ID3D11DeviceContext*					m_pContext = { nullptr };
	vector<ID3D11ShaderResourceView*>		m_Textures[AI_TEXTURE_TYPE_MAX];

public:
	static CAIMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMaterial* _pMaterial, const _char* _strModelFilePath);
	virtual void Free() override;
};

NS_END