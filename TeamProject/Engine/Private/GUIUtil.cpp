#include "Engine_Defines.h"
#include "GUIUtil.h"

void GuiUtil::ShowListButton(const vector<string>& vector, function<void(const string&)> callback)
{
	for (const string& btnName : vector)
	{
		if (ImGui::Button(btnName.c_str()))
		{
			callback(btnName);
		}
	}
}

void GuiUtil::ShowListString(const vector<string>& vector, function<void(const string&)> callback)
{

	for (auto it = vector.begin(); it != vector.end(); ++it)
	{
		string ListID = "##" + *it;
		ImGui::PushID(ListID.c_str());
		if (ImGui::Selectable(it->c_str()))
		{
			callback(*it);
		}
		ImGui::PopID();
	}
}

void GuiUtil::ShowListInt(const vector<string>& vector, function<void(_uint)> callback)
{
	for (size_t i = 0; i < vector.size(); ++i) {
		string ListID = "##" + i;
		ImGui::PushID(ListID.c_str());
		if (ImGui::Selectable(vector[i].c_str()))
		{
			callback(i);
		}
		ImGui::PopID();
	}
}

_vector GuiUtil::Vector4Float(const string& Name, _fvector vector, bool Editable)
{
	ImGui::Text(Name.c_str());
	ImGui::DragFloat4(("##" + Name).c_str(), (float*)&vector, 0.1f);
	return vector;
}

void GuiUtil::ShowCombo(const vector<string>& vector, int currentIndex, const string& key, function<void(_uint)> callback)
{
	if (vector.empty())
		return;

	float childWidth = ImGui::GetContentRegionAvail().x;
	ImGui::SetNextItemWidth(childWidth);

	if (ImGui::BeginCombo(string("##" + key).c_str(), vector[currentIndex].c_str())) {
		for (int i = 0; i < vector.size(); ++i) {
			bool isSelected = (i == currentIndex);

			if (ImGui::Selectable(vector[i].c_str(), isSelected)) {
				currentIndex = i;
				callback(i);
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}


ConfirmResult GuiUtil::DrawConfirmPopupModal(const char* popupId, const char* title, initializer_list<const char*> lines, const char* okLabel, const char* cancelLabel, float buttonW)
{
	if (!ImGui::BeginPopupModal(popupId, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		return ConfirmResult::None;

	if (title && title[0])
	{
		ImGui::TextUnformatted(title);
		ImGui::Separator();
	}

	for (auto* s : lines)
		ImGui::TextUnformatted(s);

	ImGui::Separator();

	ConfirmResult r = ConfirmResult::None;

	if (ImGui::Button(okLabel, ImVec2(buttonW, 0.f)))
	{
		r = ConfirmResult::Ok;
		ImGui::CloseCurrentPopup();
	}

	ImGui::SameLine();

	if (ImGui::Button(cancelLabel, ImVec2(buttonW, 0.f)))
	{
		r = ConfirmResult::Cancel;
		ImGui::CloseCurrentPopup();
	}

	ImGui::EndPopup();
	return r;
}

bool GuiUtil::DrawOkPopupModal(const char* popupId, const char* title, initializer_list<const char*> lines, const char* okLabel, float buttonW)
{
	if (!ImGui::BeginPopupModal(popupId, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		return false;

	if (title && title[0])
	{
		ImGui::TextUnformatted(title);
		ImGui::Separator();
	}

	for (auto* s : lines)
		ImGui::TextUnformatted(s);

	ImGui::Separator();

	bool closed = false;

	if (ImGui::Button(okLabel, ImVec2(buttonW, 0.f)))
	{
		closed = true;
		ImGui::CloseCurrentPopup();
	}

	ImGui::EndPopup();
	return closed;
}

bool GuiUtil::DrawOkPopupModalText(const char* popupId, const char* title, const string& bodyText, const char* okLabel, float buttonW)
{
	if (!ImGui::BeginPopupModal(popupId, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		return false;

	if (title && title[0])
	{
		ImGui::TextUnformatted(title);
		ImGui::Separator();
	}

	if (!bodyText.empty())
		ImGui::TextUnformatted(bodyText.c_str());

	ImGui::Separator();

	bool closed = false;

	if (ImGui::Button(okLabel, ImVec2(buttonW, 0.f)))
	{
		closed = true;
		ImGui::CloseCurrentPopup();
	}

	ImGui::EndPopup();
	return closed;
}