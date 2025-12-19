#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class IUI_Service;
class Engine_Math;
NS_END

NS_BEGIN(UITool)

class CUIObject_Tool abstract : public CUI_Object
{
protected:
	CUIObject_Tool();
	CUIObject_Tool(const CUIObject_Tool& rhs);
	virtual ~CUIObject_Tool() DEFAULT;

public:
	virtual HRESULT Initialize(INIT_DESC* pArg = nullptr) override;
	virtual void Render_GUI() override;

public:
	void DestroyChild_FromParent();

public:
	virtual void ToJson(json& data);
	virtual void FromJson(const json& data);

protected: 
	void Add_Child(CUIObject_Tool* pChild);
	void Remove_Child(CUIObject_Tool* pChild);

	void FromJson_RefreshCount(_uint& iCount);

	virtual void Render_GUI_Layout();
	virtual void Render_GUI_Transform();
	virtual void Render_GUI_Animation();

	void Play_Animation(_float dt);
	void Set_Animation(_uint iIndex);

	void Change_Texture(_uint index, const string& levelKey, const string& TextureKey, string& OutstrTextureKey);

private:
	void ToJson_Common(json& data);
	void ToJson_Parent(json& data);
	void FromJson_LinkParent(const json& data); 

protected:
	CUIObject_Tool*		m_pParent = {};
	_int				m_iChildIndex = { -1 }; 

	_float4				m_vColor = { 1.f, 1.f, 1.f, 1.f };

	_bool				m_isBlending = {};
	_float				m_fBlendTime = {};
	_float				m_fBlendDuration = {};

	vector<UI_ANIM_CLIP> m_AnimClips;				// 애니메이션을 따로 클래스로 만드는게 나은가

	_int				m_iCurrentClipIndex = { -1 };

public:
	virtual void Free();
};

NS_END