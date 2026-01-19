#include "Engine_Defines.h"
#include "GUIUtil.h"

namespace
{
	struct ScopedColor
	{
		int count = 0;

		ScopedColor(bool enabled)
		{
			if (!enabled) return;

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.25f, 0.25f, 1.f));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.35f, 0.10f, 0.10f, 1.f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.45f, 0.12f, 0.12f, 1.f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.55f, 0.14f, 0.14f, 1.f));
			count = 4;
		}

		~ScopedColor()
		{
			if (count > 0) ImGui::PopStyleColor(count);
		}
	};
}

namespace GuiUtil
{
	ENGINE_DLL void BeginTwoColTable(const char* id)
	{
		ImGui::BeginTable(id, 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_PadOuterX);
		ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.f);
		ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
	}

	ENGINE_DLL void EndTwoColTable()
	{
		ImGui::EndTable();
	}

	ENGINE_DLL void RowLabel(const char* label)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);
		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(-FLT_MIN);
	}
	 
	ENGINE_DLL bool DrawFloatRow(const char* label, const char* id, float* v, float init, float speed, float minV, float maxV, const char* fmt)
	{
		const bool changed = (*v != init);
		ScopedColor color(changed);

		RowLabel(label);

		const float full = ImGui::GetContentRegionAvail().x;
		const float btnW = 28.f;
		const float gap = ImGui::GetStyle().ItemInnerSpacing.x;

		bool edited = false;

		if (changed)
		{
			ImGui::SetNextItemWidth(btnW);
			ImGui::PushID(id);
			if (ImGui::SmallButton("R")) { *v = init; edited = true; }
			ImGui::PopID();

			ImGui::SameLine(0.f, gap);
			ImGui::SetNextItemWidth(full - (btnW + gap));
		}
		else
			ImGui::SetNextItemWidth(full);

		edited |= ImGui::DragFloat(id, v, speed, minV, maxV, fmt);
		return edited;
	}

	ENGINE_DLL bool DrawBoolRow(const char* label, const char* id, bool* v, bool init)
	{
		const bool changed = (*v != init);
		ScopedColor color(changed);

		RowLabel(label);
		return ImGui::Checkbox(id, v);
	}
}

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