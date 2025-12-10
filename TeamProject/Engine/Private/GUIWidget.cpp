#include "GUIWidget.h"

void GUIWidget::ShowListButton(const vector<string>& vector, function<void(const string&)> callback)
{
	for (const string& btnName : vector)
	{
		if (ImGui::Button(btnName.c_str()))
		{
			callback(btnName);
		}
	}
}

void GUIWidget::ShowListString(const vector<string>& vector, function<void(const string&)> callback)
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

void GUIWidget::ShowListInt(const vector<string>& vector, function<void(_uint)> callback)
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

_vector GUIWidget::Vector4Float(const string& Name, _fvector vector, bool Editable)
{
	ImGui::Text(Name.c_str());
	ImGui::DragFloat4(("##" + Name).c_str(), (float*)&vector, 0.1f);
	return vector;
}

void GUIWidget::ShowCombo(const vector<string>& vector, int currentIndex, const string& key, function<void(_uint)> callback)
{
	if (vector.empty())
		return;

	float childWidth = ImGui::GetContentRegionAvail().x;//->이건 넓이 설정
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
