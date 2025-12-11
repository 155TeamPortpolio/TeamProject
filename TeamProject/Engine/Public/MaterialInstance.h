#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class ENGINE_DLL CMaterialInstance final : public CBase
{

private:
	CMaterialInstance(class CMaterialData* pData, ID3D11Device* pDevice);
	CMaterialInstance(const CMaterialInstance& rhs);
	~CMaterialInstance() DEFAULT;

public:
	void ApplyData(ID3D11DeviceContext* pContext);
	HRESULT Create_CBuffer(ID3D11Device* pDevice);

public:
	const string& Get_PassConstant();
	class CShader* Get_Shader();
	_uint Get_ShaderID();
	_uint Get_MaterialDataID();
	class CMaterialData* Get_MaterialData() { return m_pMaterialData; }	
	const string& Get_MaterialName();

public:
	HRESULT Set_Param(const string& ConstantName, const SHADER_PARAM& parameter);
	SHADER_PARAM* Get_Param(const string& ConstantName);

	HRESULT Override_Pass(const string& passConstant) { override_Pass = passConstant; return S_OK; };
	HRESULT Reset_Pass();
	HRESULT Override_Constant(const MaterialConstants& materialConstant);
	HRESULT Reset_Constant();

public:
	vector<_uint>& Get_TextureIndex() { return m_TextureIndexs; }
	void ChangeTexture(TEXTURE_TYPE type, _uint index);
	void Set_Blended(_bool blend) { m_IsBlended = blend; };
	_bool IsBlened() { return m_IsBlended; }

public:
	virtual void Render_GUI();

private:
	_bool m_IsBlended = { false };
	vector<_uint> m_TextureIndexs;
	unordered_map<string, SHADER_PARAM> m_DynamicSlots;

	string override_Pass = {  };

	MaterialConstants overrides_Constant = {};
	class CMaterialData* m_pMaterialData = { nullptr };
	ID3D11Buffer* m_pCBuffer = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };

public:
	static CMaterialInstance* Make_Handle(class CMaterialData* pData, ID3D11Device* pDevice);
	static CMaterialInstance* Create_Handle(const string& materialKey, const string& DefualtpassConstant, ID3D11Device* pDevice);
	CMaterialInstance* Clone();
	virtual void Free() override;
};

NS_END