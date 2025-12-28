#pragma once

#define UI    CGameInstance::GetInstance()->Get_UIMgr()
#define RES   CGameInstance::GetInstance()->Get_ResourceMgr()
#define FONT  CGameInstance::GetInstance()->Get_FontSystem()
#define CLICK CGameInstance::GetInstance()->Get_ClickMgr()
#define PROTO CGameInstance::GetInstance()->Get_PrototypeMgr()

#ifdef _DEBUG
#define ASSERT_MSG(cond, msgExpr) do { if (!(cond)) { const string __msg = (msgExpr); OutputDebugStringA((__msg + "\n").c_str()); assert(cond); } } while (0)
#else
#define ASSERT_MSG(cond, msgExpr) do { (void)sizeof(cond); } while (0)
#endif

NS_BEGIN(Client)

namespace UILoader
{
	void RegisterUI(const string& levelKey);
}

NS_END