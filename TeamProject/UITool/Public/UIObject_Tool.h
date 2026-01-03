#pragma once
#include "UI_Object.h"

NS_BEGIN(Engine)
class IUI_Service; class Engine_Math;
NS_END

NS_BEGIN(UITool)

enum class UISizeMode { Default, FHD, QHD, UHD };

class CUIObject_Tool abstract : public CUI_Object
{
protected:
	CUIObject_Tool() {}
	CUIObject_Tool(const CUIObject_Tool& rhs) : CUI_Object(rhs) {}
	virtual ~CUIObject_Tool() DEFAULT;

public:
	virtual HRESULT Initialize(INIT_DESC* pArg = nullptr) override;
	virtual void Awake() override;
	virtual void Update(_float dt) override;
	virtual void Render_GUI() override;

public:
	virtual void OnClick() override;

public:
	void  Remove_SelfFromParent();					
	void  Set_OriginTexSize(_bool enable) { m_useOriginTexSize = enable; }
	_bool Get_OriginTexSize() const       { return m_useOriginTexSize; }

public:
	virtual void Save(nlohmann::ordered_json& data) override;
	virtual void Load(const nlohmann::ordered_json& data) override;

protected:
	virtual void Render_GUI_Property();
	virtual void Render_GUI_Layout();				
	virtual void Render_GUI_Transform();			
	virtual void Render_GUI_Animation();			
	virtual void Render_GUI_Color();
	virtual void Render_GUI_Image(string& strTextureKey);

	virtual void ApplySpriteTexture(_uint idx, const string& levelKey, const string& texKey, _bool applyOriginSize);

	/*앵커 기준점에 따라 자동정렬을 하기 위해 앵커오프셋 값을 반환 (사이즈를 반영한 픽셀 값 반환)*/
	_float2 Get_AnchorOffset(ANCHOR eAnchor);

	/*키 입력해서 자식 객체 업데이트 순서 조정*/
	void KeyInput_ReorderChildren();

protected:
	void Set_BasePass(const string& pass);

private:
	static string MapToStencilTestPass(const string& basePass);
	static string NormalizeToBasePass(const string& pass);

private:
	_float  GetSizeRatio(UISizeMode mode);
	void    Render_GUI_SizeBlock();

protected:
	_bool      m_useOriginTexSize = true;

	Vector2    m_sizeFHD  = {};
	UISizeMode m_sizeMode = UISizeMode::FHD;

	_bool      m_useMask  = false;
	string     m_basePass = "Opaque";

	_bool	   m_isAspectRatioLocked = {};

public:
	virtual void Free() { __super::Free(); }
};

NS_END