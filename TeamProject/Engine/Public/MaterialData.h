#pragma once
#include "Base.h"
#include "Shader.h"

NS_BEGIN(Engine)
using TEXTUERS = vector<class CTexture*>;
//using ShaderSelector = std::function<string(const string MaterialKey)>;

class ENGINE_DLL CMaterialData :
	public CBase
{
protected:
	CMaterialData();
	virtual ~CMaterialData();
public:
	HRESULT Initialize(const string& levelKey, ifstream& ifs, const string& directory);
	virtual HRESULT Link_Texture(const string& levelKey, const string& textureKey, TEXTURE_TYPE eType);
	virtual HRESULT Link_Shader(const string& levelKey, const string& shaderKey);

public:
	void ApplyData(ID3D11DeviceContext* pContext, const vector<_uint>& TextureIndexs);
	HRESULT Set_MaterialConstantBuffer(ID3D11Buffer* pCBuffer);

public:
	class CShader* Get_Shader() { return m_pShader; }
	_uint  Get_ShaderID() { return m_pShader->Get_ID(); }
	const string&  Get_PassConstant() { return m_passConstant; }
	const MaterialConstants& Get_DefaultMaterialConstant() { return m_DefaultMaterialConstant; }
	_uint  Get_MaterialDataID() { return m_MaterialDataID; }
	const string& Get_MaterialName() { return m_MaterialKey; }
	void Change_DataPass(const string& pass) { m_passConstant = pass; }

public:
	_bool Has_Texture(TEXTURE_TYPE eType);
public:
	virtual void Render_GUI();
	virtual void Render_GUI( vector<_uint>& TextureIndexs);

protected:
	string ConvertToConstant(TEXTURE_TYPE eType);

protected:
	/*Fixed Material Data Property -> CAnnot Override*/
	_uint m_MaterialDataID = {};
	string m_MaterialKey = {};
	class CShader* m_pShader = { nullptr };
	map<TEXTURE_TYPE, TEXTUERS> m_Textures;

	/*Default_Material Data Property -> Can Override*/
	string m_passConstant = {};
	MaterialConstants m_DefaultMaterialConstant = {};

public:
	static CMaterialData* Create(const string& levelKey, ifstream& ifs, const string& directory);
	static CMaterialData* Create(const string& materialKey, const string& DefualtpassConstant);
	virtual void Free() override;
	static _uint s_NextID;
public:

};

NS_END