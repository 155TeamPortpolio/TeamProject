#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class IUI_Service;
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

private:
	void ToJson_Common(json& data);
	void ToJson_Parent(json& data);
	void FromJson_LinkParent(const json& data); 

protected:
	CUIObject_Tool*		m_pParent = {};
	_int				m_iChildIndex = { -1 }; 

public:
	virtual void Free();
};

NS_END