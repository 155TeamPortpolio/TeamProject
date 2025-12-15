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
	virtual json ToJson() { return json(); }
	virtual void FromJson(const json& data) {}

	void LinkChildFromJson(const json& data);

protected:
	void ToJson_Common(json& data);
	virtual void Render_GUI_Layout();
	virtual void Render_GUI_Transform();

public:
	virtual void Free();
};

NS_END