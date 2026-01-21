#pragma once
#include "Component.h"
NS_BEGIN(Engine)
class ENGINE_DLL CTextSlot :
    public CComponent
{
    struct AnchorInfo {
        _bool bAutoPos = { false }; // 매 프레임 기준점을 재계산할지
        ANCHOR eAnchor;             // 정렬 규칙 (바뀔 때 한 번만 설정)
        _float2 vPivot;             // 기준점 (매 프레임 갱신)
    };
private:
    CTextSlot();
    CTextSlot(const CTextSlot& rhs);
    virtual ~CTextSlot() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(COMPONENT_DESC* pArg) override;

public:
    HRESULT Set_Font(const string FontTag);
    void Set_Color(_float4 color);
    void Set_Size(_float scale);
    void Set_Position(_float2 Pos);
    void Set_Rotate(_float Rotate);
    void Set_Text(wstring Text) { m_Info.Text = Text; };
    void Set_TextKey(string TextKey) { m_Info.TextKey = TextKey; };
    void Set_OutLine(_float Thickness, _float4 OutlineColor);
    void ReSet_OutLine() { m_Info.OutLined = false; };
    void Set_Origin(_float2 origin) { m_Info.Origin = origin; };
    void Set_Shear(_float2 shear) { m_Info.vShear = shear; }

public:
    void Push_Text();
    void Render_Text();
    void Set_AutoPos(ANCHOR anchor, _float2 Pivot);
    void Release_AutoPos() {
        m_AnchorInfo.bAutoPos = false;
    }

public:
    // Set_AutoPos, Set_Anchor 대신에 아래 함수 추가 

    /* 자동 정렬 기능 활성화
    - TextSlot이 외부에서 직접 좌표를 받지 않고, Anchor + Pivot 기준으로 매 프레임 위치를 자동 계산
    - 보통 Initialize 단계에서 1회 호출 */
    void Enable_AutoPos(ANCHOR anchor);
    /* 자동 정렬 기능 비활성화 
    - TextSlot 위치를 Set_Position으로 직접 제어 */
    void Disable_AutoPos();
    /* AutoPos 기준이 되는 Pivot 좌표 갱신 
    - 부모 UI의 LeftTop, Size가 변할 수 있으므로 보통 Update에서 호출 
    - Pivot : 정렬 기준점 (ex. 부모 UI의 중심) */
    void Update_Pivot(_float2 pivot);
    void Set_Anchor(ANCHOR anchot, _float2 Pivot);
    _float2 Get_Anchor(ANCHOR anchot);
    _float2 Get_TextSize();
    _float Get_Scale();
    _float2 Get_Shear() { return m_Info.vShear; }

private:
    class CCustomFont* m_pFont = { nullptr };
    TEXT_INFO m_Info = { };
    AnchorInfo m_AnchorInfo = {};
    _bool isOutLined = { false };

private:
    void Apply_Align();

public:
    static CTextSlot* Create();
    virtual CComponent* Clone() override;
    virtual void Free() override;
};

NS_END