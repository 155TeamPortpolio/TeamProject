#pragma once
#include "Engine_Defines.h"

enum class ConfirmResult
{
	None, Ok, Cancel
};

namespace GuiUtil
{
	ENGINE_DLL void BeginTwoColTable(const char* id);
	ENGINE_DLL void EndTwoColTable();
	ENGINE_DLL void RowLabel(const char* label);

	ENGINE_DLL bool DrawFloatRow(const char* label, const char* id, float* v, float init, float speed, float minV, float maxV, const char* fmt);
	ENGINE_DLL bool DrawBoolRow(const char* label, const char* id, bool* v, bool init);

     void ShowListButton(const vector<string>& vector, function<void(const string&)> callback);
     void ShowListString(const vector<string>& vector, function<void(const string&)> callback);
     void ShowListInt(const vector<string>& vector, function<void(_uint)> callback);
     _vector Vector4Float(const string& Name,_fvector vector, bool Editable = true);
     void ShowCombo(const vector<string>& vector, int currentIndex, const string& key, function<void(_uint)> callback);

	 // 확인(Ok, Cancel) 팝업. lines 내용을 출력하고, 버튼 선택 결과(Ok/Cancel/None)를 반환.
	 ENGINE_DLL ConfirmResult DrawConfirmPopupModal(const char* popupId, const char* title, initializer_list<const char*> lines, const char* okLabel, const char* cancelLabel, float buttonW = 120.f);

	 // 단순 확인(OK만) 팝업. lines 내용을 출력하고, Ok를 눌러 닫히면 true(그 외엔 false).
	 ENGINE_DLL bool DrawOkPopupModal(const char* popupId, const char* title, initializer_list<const char*> lines,
		 const char* okLabel = "OK", float buttonW = 120.f);

	 // 단순 확인(Ok만) 팝업(본문이 string). bodyText를 출력하고, Ok를 눌러 닫히면 true(그외엔 false)
	 ENGINE_DLL bool DrawOkPopupModalText(const char* popupId, const char* title, const string& bodyText,
		 const char* okLabel = "OK", float buttonW = 120.f);

	 ENGINE_DLL void PushTypeBadgeColor(const std::string& typeName);
	 ENGINE_DLL void PopTypeBadgeColor();
	 ENGINE_DLL void DrawBoundDot(bool isBound);
	 
}