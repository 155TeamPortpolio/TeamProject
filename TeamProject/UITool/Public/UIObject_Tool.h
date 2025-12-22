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
	void Remove_SelfFromParent();					// (툴) 자신을 자식으로 가진 부모 컨테이너에서 자신을 지움

public:
	virtual void ToJson(json& data);				// (툴) json으로 쓰기
	virtual void FromJson(const json& data);		// (툴, 클라이언트) json에서 읽기

public:
	virtual void SavePrefab(json& data);
	virtual void LoadPrefab(const json& data);

protected: 
	void FromJson_RefreshCount(_uint& iCount);		// (툴) json에서 읽을 때 오브젝트 개수를 새로고침

protected:
	virtual void Render_GUI_Layout();				// (툴) GUI 앵커 오프셋, 사이즈
	virtual void Render_GUI_Transform();			// (툴) GUI 스케일, 앵글, 피봇
	virtual void Render_GUI_Animation();			// (툴)	GUI 애니메이션 추가
	virtual void Render_GUI_Color();
	virtual void Render_GUI_TextKey(); 

protected:
	void Play_Animation(_float dt);					// (툴, 클라이언트)			
	void Set_Animation(_uint iIndex);				// (툴, 클라이언트)

protected:
	void Change_Texture(_uint index, const string& levelKey, const string& TextureKey, string& OutstrTextureKey);	// (툴) (아마 없어질 듯)
	_int Find_TextureIndex(const vector<const _char*> TextureKeys, const string strTextureTag);	// (툴)

private:
	void ToJson_Common(json& data);					// (툴)	
	void ToJson_Parent(json& data);					// (툴)
	void ToJson_Animation(json& data);				// (툴)

	void FromJson_Parent(const json& data);			// (툴, 클라이언트)
	void FromJson_Animation(const json& data);		// (툴, 클라이언트)

	void Save_Transform(json& data);
	void Save_TextKey(json& data);
	void Save_Animation(json& data);
	void Save_Childeren(json& data);

	void Load_Transform(const json& data);
	void Load_TextKey(const json& data);
	void Load_Animation(const json& data);
	void Load_Children(const json& data);

	class CUI_Object* CreateChildObject(const json& data, const string& strLevelKey);

	void Reset_Animation();

protected:
	_bool				m_isBlending = {};
	_float				m_fBlendTime = {};
	_float				m_fBlendDuration = {};

	vector<UI_ANIM_CLIP> m_AnimClips;
	_int				m_iCurrentClipIndex = { -1 };

	_float2				m_vBaseScale = {};
	_float				m_vBaseAngle = {};
	_float4				m_vBaseColor = {};

	_char				m_szTextKey[MAX_PATH] = {};

public:
	virtual void Free();
};

NS_END