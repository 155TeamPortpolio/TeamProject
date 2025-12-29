#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class IUI_Service;
class Engine_Math;
NS_END

NS_BEGIN(UITool)

enum class UISizeMode { FHD, QHD, UHD };

class CUIObject_Tool abstract : public CUI_Object
{
protected:
	CUIObject_Tool() {}
	CUIObject_Tool(const CUIObject_Tool& rhs) : CUI_Object(rhs) {}
	virtual ~CUIObject_Tool() DEFAULT;

public:
	virtual HRESULT Initialize(INIT_DESC* pArg = nullptr) override;
	virtual void Render_GUI() override;

public:
	void  Remove_SelfFromParent();					// (툴) 자신을 자식으로 가진 부모 컨테이너에서 자신을 지움
	void  Set_OriginTexSize(_bool enable) { m_useOriginTexSize = enable; }
	_bool Get_OriginTexSize() const       { return m_useOriginTexSize; }

public:
	virtual void FillElementData(UI_ELEMENT_DATA& data) override;
	virtual void ReadElementData(const UI_ELEMENT_DATA& data) override;

protected:
	virtual void Render_GUI_Property();
	virtual void Render_GUI_Layout();				// (툴) GUI 앵커 오프셋, 사이즈
	virtual void Render_GUI_Transform();			// (툴) GUI 스케일, 앵글, 피봇
	virtual void Render_GUI_Animation();			// (툴)	GUI 애니메이션 추가
	virtual void Render_GUI_Color();

	virtual void ApplySpriteTexture(_uint idx, const string& levelKey, const string& texKey, _bool applyOriginSize);

protected:
	_int Find_TextureIndex(const vector<const _char*> TextureKeys, const string strTextureTag);	// (툴)

	_bool m_useOriginTexSize = true;

	Vector2    m_sizeFHD  = {};
	UISizeMode m_sizeMode = UISizeMode::FHD;

public:
	virtual void Free();
};

NS_END