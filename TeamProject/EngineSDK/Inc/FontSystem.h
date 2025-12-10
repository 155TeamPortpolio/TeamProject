#pragma once
#include "IFontService.h"
NS_BEGIN(Engine)
class CFontSystem :
    public IFontService
{

    struct TextData {
        TEXT_INFO info = {};
        _uint systemIndex = {};
    };

private:
    CFontSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CFontSystem() DEFAULT;

public:
    HRESULT Initialize();

public:
    virtual HRESULT Add_Font(string FontName, const wstring& FontFilePath);
    virtual class CCustomFont* Get_Font(string FontName);
    virtual HRESULT Push_Text(TEXT_INFO info);

    virtual HRESULT Render_Font();

private:
    ID3D11Device* m_pDevice = {nullptr};
    ID3D11DeviceContext* m_pContext = {nullptr};
    SpriteBatch* m_pBatch = { nullptr };
    CommonStates * m_pStates = { nullptr };
    vector<class CCustomFont*> m_Fonts;
    unordered_map<string, _uint> m_FontsIndex;

    vector<TextData> m_Texts;
public:
    static class CFontSystem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};
NS_END
