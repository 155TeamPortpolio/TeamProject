#pragma once
#include "UIObject_Tool.h"

NS_BEGIN(UITool)

class CImageUI final : public CUIObject_Tool
{
private:
	CImageUI();
	CImageUI(const CImageUI& rhs);
	virtual ~CImageUI() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(INIT_DESC* pArg = nullptr) override;

	virtual void Priority_Update(_float dt) override;
	virtual void Update(_float dt) override;
	virtual void Late_Update(_float dt) override;

	virtual void Render_GUI() override;

public:
	virtual void ToJson(json& data) override;
	virtual void FromJson(const json& data) override;

private:
	string		m_strTextureKey;
	_int		m_iTextureKeyIndex = { 0 };		// gui에 콤보박스에서 텍스쳐 선택했을 때 인덱스

public:
	static _uint m_iCount;

private:
	void Change_Texture(_uint index, const string& levelKey, const string& TextureKey);

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free();
};

NS_END