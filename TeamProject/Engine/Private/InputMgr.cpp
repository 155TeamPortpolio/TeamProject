#include "Engine_Defines.h"
#include "InputMgr.h"

CInputMgr::CInputMgr()
{
}

CInputMgr::~CInputMgr()
{
}

HRESULT CInputMgr::Initialize(HWND hwnd)
{
    m_hWnd = hwnd;
	m_Keyboard.resize(256);

    RAWINPUTDEVICE rid[2];

	// 키보드
	rid[0].usUsagePage = 0x01;   // 일반 장치
	rid[0].usUsage = 0x06;       // 키보드
	rid[0].dwFlags = 0;          // 기본 처리 방식
	rid[0].hwndTarget = m_hWnd;    // 이 윈도우로 메시지 보내줘

	// 마우스
	rid[1].usUsagePage = 0x01;
	rid[1].usUsage = 0x02;       // 마우스
	rid[1].dwFlags = 0;
	rid[1].hwndTarget = m_hWnd;

	RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE));

	return S_OK;
}

void CInputMgr::Update()
{
	m_Mouse.fDeltaX = 0.f;
	m_Mouse.fDeltaY = 0.f;
	m_Mouse.fWheelDelta = 0.f;

	for (auto& Key_ID : m_Keyboard) {
		if (Key_ID.CurrDown && !Key_ID.PrevDown)
			Key_ID.state = KEY_STATE::TAP;

		else if(Key_ID.CurrDown && Key_ID.PrevDown)
			Key_ID.state = KEY_STATE::HOLD;

		else if(!Key_ID.CurrDown && Key_ID.PrevDown)
			Key_ID.state = KEY_STATE::AWAY;

		else if(!Key_ID.CurrDown && !Key_ID.PrevDown)
			Key_ID.state = KEY_STATE::NONE_KEY;

		Key_ID.PrevDown = Key_ID.CurrDown;
	}

	for (auto& Mouse_ID : m_Mouse.mouseKey) {
		if (Mouse_ID.CurrDown && !Mouse_ID.PrevDown)
			Mouse_ID.state = KEY_STATE::TAP;

		else if (Mouse_ID.CurrDown && Mouse_ID.PrevDown)
			Mouse_ID.state = KEY_STATE::HOLD;

		else if (!Mouse_ID.CurrDown && Mouse_ID.PrevDown)
			Mouse_ID.state = KEY_STATE::AWAY;

		else if (!Mouse_ID.CurrDown && !Mouse_ID.PrevDown)
			Mouse_ID.state = KEY_STATE::NONE_KEY;

		Mouse_ID.PrevDown = Mouse_ID.CurrDown;
	}

	GetCursorPos(&m_pMousePos);
	ScreenToClient(m_hWnd, &m_pMousePos);

	if (m_Keyboard['A'].state == KEY_STATE::TAP)
		OutputDebugStringA("A Key Down\n");

	if (m_Keyboard['A'].state == KEY_STATE::HOLD)
		OutputDebugStringA("A Key Hold\n");

	if (m_Keyboard['A'].state == KEY_STATE::AWAY)
		OutputDebugStringA("A Key Up\n");
}

const _float2& CInputMgr::Mouse_Pos()
{
	return { (float)m_pMousePos.x, (float)m_pMousePos.y };
}

void CInputMgr::Process_Input(LPARAM lParam)
{
	messageSize = 0;

	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &messageSize, sizeof(RAWINPUTHEADER));
	if (m_vecInputMsg.size() < messageSize)
		m_vecInputMsg.resize(messageSize);

	if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, m_vecInputMsg.data(), &messageSize, sizeof(RAWINPUTHEADER)) == messageSize)
	{
		RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(m_vecInputMsg.data());
		Process_RawInput(raw);
	}
}

void CInputMgr::Process_RawInput(RAWINPUT* raw)
{
		if (raw == nullptr)
			return;

		// 어떤 장치인지 확인
		switch (raw->header.dwType)
		{
		case RIM_TYPEKEYBOARD:
			HandleKeyboardInput(raw->data.keyboard);
			break;

		case RIM_TYPEMOUSE:
			HandleMouseInput(raw->data.mouse);
			break;
		}
}

void CInputMgr::HandleKeyboardInput(const RAWKEYBOARD& keyboard)
{
	USHORT flags = keyboard.Flags;
	USHORT vkey = keyboard.VKey;

	bool keyDown = !(flags & RI_KEY_BREAK); // 0이면 눌림, 1이면 뗌

	if (keyDown)
		m_Keyboard[vkey].CurrDown =  true;
	else
		m_Keyboard[vkey].CurrDown = false;
}


void CInputMgr::HandleMouseInput(const RAWMOUSE& mouse)
{
	if (mouse.usFlags == MOUSE_MOVE_RELATIVE) {
		m_Mouse.fDeltaX += mouse.lLastX;
		m_Mouse.fDeltaY += mouse.lLastY;
	}

	// 좌클릭
	if (mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
		m_Mouse.mouseKey[0].CurrDown = true;
	if (mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
		m_Mouse.mouseKey[0].CurrDown = false;

	// 우클릭
	if (mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
		m_Mouse.mouseKey[1].CurrDown = true;
	if (mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
		m_Mouse.mouseKey[1].CurrDown = false;

	// 중클릭
	if (mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
		m_Mouse.mouseKey[2].CurrDown = true;
	if (mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
		m_Mouse.mouseKey[2].CurrDown = false;
	
	//휠
	if (mouse.usButtonFlags & RI_MOUSE_WHEEL)
	{
		SHORT zDelta = (SHORT)mouse.usButtonData;
		m_Mouse.fWheelDelta = zDelta / WHEEL_DELTA; //ms 휠 단위
	}
}

CInputMgr* CInputMgr::Create(HWND hwnd)
{
	CInputMgr* instance = new CInputMgr();

	if (FAILED(instance->Initialize(hwnd))) {
		Safe_Release(instance);
	}

	return instance;
}

void CInputMgr::Free()
{
	__super::Free();
}
