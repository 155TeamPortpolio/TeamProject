#pragma once

NS_BEGIN(CameraTool)

namespace CamPanelUtil
{
    bool TableCellHit(const char* id, float rowH, bool& outRowHovered, bool& outRowClicked);

	void DrawLabelDisabled(const char* t);
	bool DragFloat(const char* id, float& v, float speed, float minV, float maxV, const char* fmt, float valueW);

	template<int N>
	inline bool DragFloatN(const char* id, const char* const (&labels)[N], float* const (&values)[N], float speed, float minV, float maxV, const char* fmt, float valueW, float gap)
	{
		bool changed = false;

		ImGui::PushID(id);

		for (int i = 0; i < N; ++i)
		{
			DrawLabelDisabled(labels[i]);
			ImGui::SameLine();

			char compId[16];
			sprintf_s(compId, "##%d", i);

			changed |= DragFloat(compId, *values[i], speed, minV, maxV, fmt, valueW);

			if (i + 1 < N) ImGui::SameLine(0.f, gap);
		}

		ImGui::PopID();
		return changed;
	}

	inline bool DragFloat1(const char* id, float& v, float speed, float minV, float maxV, const char* fmt, float valueW)
	{
		const char* const labels[1] = { "" };
		float* const values[1] = { &v };
		return DragFloatN<1>(id, labels, values, speed, minV, maxV, fmt, valueW, 0.f);
	}
	inline bool DragVec2XY(const char* id, _vector2& v, float speed, float minV, float maxV, const char* fmt, float valueW, float gap)
	{
		const char* const labels[2] = { "X", "Y" };
		float* const values[2] = { &v.x, &v.y };
		return DragFloatN<2>(id, labels, values, speed, minV, maxV, fmt, valueW, gap);
	}

	inline bool DragVec3XYZ(const char* id, _vector3& v, float speed, float minV, float maxV, const char* fmt, float valueW, float gap)
	{
		const char* const labels[3] = { "X", "Y", "Z" };
		float* const values[3] = { &v.x, &v.y, &v.z };
		return DragFloatN<3>(id, labels, values, speed, minV, maxV, fmt, valueW, gap);
	}

	inline bool DragVec4XYZW(const char* id, _vector4& v, float speed, float minV, float maxV, const char* fmt, float valueW, float gap)
	{
		const char* const labels[4] = { "X", "Y", "Z", "W" };
		float* const values[4] = { &v.x, &v.y, &v.z, &v.w };
		return DragFloatN<4>(id, labels, values, speed, minV, maxV, fmt, valueW, gap);
	}

	inline bool DragVec3(const char* id, _vector3& v, const char* lx, const char* ly, const char* lz, float speed, float minV, float maxV, const char* fmt, float valueW, float gap)
	{
		const char* const labels[3] = { lx, ly, lz };
		float* const values[3] = { &v.x, &v.y, &v.z };
		return DragFloatN<3>(id, labels, values, speed, minV, maxV, fmt, valueW, gap);
	}
}

NS_END