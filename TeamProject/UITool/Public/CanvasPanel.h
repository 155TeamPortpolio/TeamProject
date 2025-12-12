#pragma once
#include "UI_Object.h"

// 캔버스 패널은 모든 ui 오브젝트의 최상위 객체로
// 목적은 UI가 항상 화면 전체를 기준으로 작동하도록 화면에 동기화 (화면 크기에 맞춰서 자동으로 크기 맞춤)

NS_BEGIN(UITool)

class CCanvasPanel final : public CUI_Object
{
private:
	CCanvasPanel();
	CCanvasPanel(const CCanvasPanel& rhs);
	virtual ~CCanvasPanel() DEFAULT;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(INIT_DESC* pArg = nullptr) override;

	virtual void Priority_Update(_float dt) override;
	virtual void Update(_float dt) override;
	virtual void Late_Update(_float dt) override;

	virtual void Render_GUI() override;

public:
	static CGameObject* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg = nullptr) override;
	virtual void Free();
};

NS_END