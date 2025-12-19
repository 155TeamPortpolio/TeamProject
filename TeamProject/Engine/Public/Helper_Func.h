#pragma once
#include "Engine_Defines.h"
#include <random>

#include "Engine_Math.h"
#include <magic_enum_Inc/magic_enum.hpp>

namespace Helper
{
	//아스키 코드가 아닌 것이 포함되어 있는지 확인
	ENGINE_DLL _bool ContainsNonAscii(const string& str);
	//솔루션 폴더 안에 있는 것인지 확인
	ENGINE_DLL _bool IsPathInProjectFolder(const string& path);
	//파일 선택용 탐색기
	ENGINE_DLL string OpenFile_Dialogue();
	//폴더 선택용 탐색기
	ENGINE_DLL string OpenFolder_Dialogue();
	//여러 폴더 가져오기
	ENGINE_DLL vector<string> OpenMultiFolders();
	//여러 파일 가져오기
	ENGINE_DLL vector<string> OpenMultiFiles();
	//파일 저장용 탐색기
	ENGINE_DLL string SaveFileDialog();
	//파일 저장용 탐색기 - nfd기반
	ENGINE_DLL string SaveFileDialog(const string& fileName, const string& filter);
	//파일 저장용 탐색기 - 기본 파일 형식, 이름 지정 가능
	ENGINE_DLL string SaveFileDialogByWinAPI(const string& fileName, const string& filterStr);
	//wstring으로 변환
	ENGINE_DLL wstring ConvertToWideString(const string& str);
	//string으로 변환
	ENGINE_DLL string ConvertToString(const wstring& wstr);
	//파일명+확장자 추출
	ENGINE_DLL string GetFileNameWithExtension(const string& filePath);
	//확장자명 제외, 파일명 추출
	ENGINE_DLL string GetFileNameWithOutExtension(const string& filePath);
	//모두 소문자로 변환
	ENGINE_DLL string  ToLower(const string& fileName);
	//경로에 텍스처 저장
	ENGINE_DLL HRESULT SaveTextureToDDs(ID3D11DeviceContext* pContext, const string& filePath, ID3D11ShaderResourceView* pSRV);
	ENGINE_DLL HRESULT SaveTextureToDDs(ID3D11DeviceContext* pContext, const wstring& filePath, ID3D11ShaderResourceView* pSRV);
	//랜덤 함수
	ENGINE_DLL _int Get_Random_Int(_int min, _int max);
	ENGINE_DLL _float Get_Random_Float(_float min, _float max);

	/*해당 디렉토리에 있는지*/
	ENGINE_DLL _bool IsUnderDirectory(const filesystem::path& file, const filesystem::path& dir);
}

//json 저장 및 불러오기
namespace Helper
{
	template <typename T>
	inline T LoadJson(const string& filePath) {
		std::ifstream file(filePath);
		if (!file.is_open())
			return T{};

		json j;
		file >> j;
		return j.get<T>();
	};

	template <typename T>
	inline void SaveJson(T& Data, const string& filePath) {
		//json JsonData = Data;
		nlohmann::ordered_json JsonData = Data;
		ofstream file(filePath);

		if (file.is_open()) {
			file << JsonData.dump(2);
			file.close();
		}
	};
}

namespace Helper // magic_enum 관련
{
	template<typename TEnum>
	string_view EnumNameView(TEnum v)
	{
		static_assert(is_enum_v<TEnum>, "Helper::EnumNameView<TEnum> requires an enum type.");
		return magic_enum::enum_name(v);
	}

	template<typename TEnum>
	string EnumToString(TEnum v)
	{
		static_assert(is_enum_v<TEnum>, "Helper::EnumToString<TEnum> requires an enum type.");
		return string(EnumNameView(v));
	}

	template<typename TEnum>
	const char* EnumLabel(TEnum v)
	{
		static_assert(is_enum_v<TEnum>, "Helper::EnumLabel<TEnum> requires an enum type.");

		constexpr size_t N = magic_enum::enum_count<TEnum>();

		struct Cache
		{
			array<string, N> labels{};

			Cache()
			{
				auto values = magic_enum::enum_values<TEnum>();
				for (size_t i = 0; i < N; ++i) labels[i] = string(magic_enum::enum_name(values[i]));
			}

			const char* Get(TEnum x) const
			{
				return labels[magic_enum::enum_index(x).value()].c_str();
			}
		};

		static Cache cache;
		return cache.Get(v);
	}

	// enum 선택 콤보박스(UI)를 그려주고 선택 결과를 반영
	template<typename TEnum, typename FilterFn>
	bool DrawEnumCombo(const char* id, TEnum& ioValue, TEnum shownValue, float width, FilterFn Filter) 
	{
		static_assert(is_enum_v<TEnum>, "Helper::DrawEnumCombo<TEnum> requires an enum type.");

		ImGui::SetNextItemWidth(width);
		if (!ImGui::BeginCombo(id, EnumLabel(shownValue))) return false;

		bool changed = false;

		for (TEnum v : magic_enum::enum_values<TEnum>())
		{
			if (!Filter(v)) continue;

			const bool selected = (shownValue == v);

			if (ImGui::Selectable(EnumLabel(v), selected)) { ioValue = v; changed = true; }
			if (selected) ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
		return changed;
	}

	// A: 가장 기본형, 현재 값을 그대로 보여주고, 선택하면 그대로 바꾸는" 가장 일반적인 상황
	template<typename TEnum>
	bool DrawEnumCombo(const char* id, TEnum& ioValue, float width)
	{
		return DrawEnumCombo(id, ioValue, ioValue, width, [](TEnum) { return true; });
	}

	// B: 표시값(shownValue)을 따로 주는 버젼 -> "콤보에 표시되는 값"과 "실제로 저장될 값"을 분리하고 싶을때
	template<typename TEnum>
	bool DrawEnumCombo(const char* id, TEnum& ioValue, TEnum shownValue, float width)
	{
		return DrawEnumCombo(id, ioValue, shownValue, width, [](TEnum) { return true; });
	}

	// C: 필터(FilterFn)로 일부 enum을 숨기는 버전(표시값 분리 포함) -> 특정 값은 UI에서 선택 못 하게 숨기고 싶을 때 (None, End는 목록에서 제외.. 등)
	template<typename TEnum, typename FilterFn>
	bool DrawEnumCombo(const char* id, TEnum& ioValue, float width, FilterFn Filter)
	{
		return DrawEnumCombo(id, ioValue, ioValue, width, Filter);
	}
}