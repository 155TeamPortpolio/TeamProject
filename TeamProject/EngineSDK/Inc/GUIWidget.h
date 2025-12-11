#pragma once
#include "Engine_Defines.h"
namespace GUIWidget
{
     void ShowListButton(const vector<string>& vector, function<void(const string&)> callback);
     void ShowListString(const vector<string>& vector, function<void(const string&)> callback);
     void ShowListInt(const vector<string>& vector, function<void(_uint)> callback);
     _vector Vector4Float(const string& Name,_fvector vector, bool Editable = true);
     void ShowCombo(const vector<string>& vector, int currentIndex, const string& key, function<void(_uint)> callback);
}