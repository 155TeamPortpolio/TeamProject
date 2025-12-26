#include "Engine_Defines.h"
#include "Helper_Func.h"
#include <shobjidl.h> 

_bool Helper::ContainsNonAscii(const string& str)
{
	for (unsigned char c : str) {
		if (c >= 0x80) {
			return true;
		}
	}
	return false;
}
 
_bool Helper::IsPathInProjectFolder(const string& path)
{
	//현재 솔루션 Path
	const auto projectPath = std::filesystem::weakly_canonical(SOLUTION_DIR);
	//선택된 프로젝트 Path
	auto selectedPath = std::filesystem::weakly_canonical(path);

	string projectStr = projectPath.string();
	string selectedStr = selectedPath.string();

	if (selectedPath == projectPath) {
		return true;
	}

	if (selectedStr.find(projectStr) == 0) // selectedStr이 projectStr로 시작하면
	{
		return true;
	}

	return true;
}

string Helper::OpenFile_Dialogue()
{
	string filePath = "";
	nfdchar_t* outPath = nullptr;
	filesystem::path defaultPath = SOLUTION_DIR;
	nfdresult_t result = NFD_OpenDialog(nullptr, defaultPath.string().c_str(), &outPath);
	if (result == NFD_OKAY)
	{
		filePath = outPath;
		if (Helper::ContainsNonAscii(filePath)) {
			MSG_BOX("File Path Must Be English");
			filePath.clear();
		}
		else if (!Helper::IsPathInProjectFolder(filePath)) {
			MSG_BOX("Files outside the project folder cannot be selected");
			filePath.clear();
		}
	}
	else if (result == NFD_ERROR)
	{
		MSG_BOX("File Open Error : CGUISystem");
	}

	if (outPath)
		free(outPath);

	return filePath;
}

string Helper::OpenFolder_Dialogue()
{
	string folderPath = "";
	nfdchar_t* outPath = nullptr;
	filesystem::path defaultPath = SOLUTION_DIR;

	nfdresult_t result = NFD_PickFolder(defaultPath.string().c_str(), &outPath);
	if (result == NFD_OKAY)
	{
		folderPath = outPath;
		if (Helper::ContainsNonAscii(folderPath)) {
			MSG_BOX("Folder Can't Be Open : Not English Path");
			folderPath.clear();
		}
		else if (!Helper::IsPathInProjectFolder(folderPath)) {
			MSG_BOX("Folders outside the project folder cannot be selected");
			folderPath.clear();
		}
	}
	else if (result == NFD_ERROR)
	{
		MSG_BOX("Folder Open Error : CGUISystem");
	}

	if (outPath)
		free(outPath);

	return folderPath;
}

ENGINE_DLL vector<string> Helper::OpenMultiFolders()
{
	vector<string> result;
	IFileOpenDialog* pFileOpen = nullptr;//(COM객체로 동작 COM 객체를 가리키는 인터페이스 )

	HRESULT Initialize = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);//(현재 스레드에서 환경 준비)
	if (FAILED(Initialize)) {
		MSG_BOX("Failed To Initialize Com Interface : CoInitializeEx");
		return result;
	}

	HRESULT hr = CoCreateInstance(
		CLSID_FileOpenDialog,      // 만들고 싶은 COM 클래스의 고유 ID
		NULL,                      // Aggregation 안 함 (대부분 NULL)
		CLSCTX_ALL,                // 실행 컨텍스트 (in-proc, out-of-proc 등)
		IID_IFileOpenDialog,       // 요청할 인터페이스의 ID
		reinterpret_cast<void**>(&pFileOpen) // 인터페이스 포인터 반환받을 변수
	);

	if (FAILED(hr)) {
		MSG_BOX("Failed To CReate Com Interface : CoCreateInstance");
		return result;
	}

	DWORD dwOptions;
	pFileOpen->GetOptions(&dwOptions);
	pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS | FOS_ALLOWMULTISELECT);

	HRESULT openDialogue = pFileOpen->Show(0);

	if (SUCCEEDED(openDialogue)) {
		IShellItemArray* pItemArray = { nullptr };  // 이것도 컴객체임 IFileOpenDialog가 반환하는 결과 배열
		HRESULT openResult = pFileOpen->GetResults(&pItemArray);

		if (SUCCEEDED(openResult)) {
			DWORD itemCount = 0;
			pItemArray->GetCount(&itemCount);

			for (DWORD i = 0; i < itemCount; i++)
			{
				IShellItem* pItem = { nullptr };
				if (SUCCEEDED(pItemArray->GetItemAt(i, &pItem)))
				{
					PWSTR pszFilePath = { nullptr };
					if (SUCCEEDED(pItem->GetDisplayName(SIGDN_PARENTRELATIVE, &pszFilePath))) {//사용자가 선택한 파일이나 폴더 하나(부모 폴더 기준으로)
						result.push_back(ConvertToString(pszFilePath));
						CoTaskMemFree(pszFilePath);// 할당한 메모리를 해제
					}
					pItem->Release();
				}
			}
			pItemArray->Release();
		}
	}
	pFileOpen->Release();

	CoUninitialize();

	return result;
}

ENGINE_DLL vector<string> Helper::OpenMultiFiles()
{
	vector<string> result;
	IFileOpenDialog* pFileOpen = nullptr;//(COM객체로 동작 COM 객체를 가리키는 인터페이스 )

	HRESULT Initialize = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);//(현재 스레드에서 환경 준비)
	if (FAILED(Initialize)) {
		MSG_BOX("Failed To Initialize Com Interface : CoInitializeEx");
		return result;
	}

	HRESULT hr = CoCreateInstance(
		CLSID_FileOpenDialog,      // 만들고 싶은 COM 클래스의 고유 ID
		NULL,                      // Aggregation 안 함 (대부분 NULL)
		CLSCTX_ALL,                // 실행 컨텍스트 (in-proc, out-of-proc 등)
		IID_IFileOpenDialog,       // 요청할 인터페이스의 ID
		reinterpret_cast<void**>(&pFileOpen) // 인터페이스 포인터 반환받을 변수
	);

	if (FAILED(hr)) {
		MSG_BOX("Failed To CReate Com Interface : CoCreateInstance");
		return result;
	}

	DWORD dwOptions;
	pFileOpen->GetOptions(&dwOptions);
	pFileOpen->SetOptions(dwOptions | FOS_FILEMUSTEXIST | FOS_PATHMUSTEXIST | FOS_ALLOWMULTISELECT);
	HRESULT openDialogue = pFileOpen->Show(0);

	if (SUCCEEDED(openDialogue)) {
		IShellItemArray* pItemArray = { nullptr };  // 이것도 컴객체임 IFileOpenDialog가 반환하는 결과 배열
		HRESULT openResult = pFileOpen->GetResults(&pItemArray);

		if (SUCCEEDED(openResult)) {
			DWORD itemCount = 0;
			pItemArray->GetCount(&itemCount);

			for (DWORD i = 0; i < itemCount; i++)
			{
				IShellItem* pItem = { nullptr };
				if (SUCCEEDED(pItemArray->GetItemAt(i, &pItem)))
				{
					PWSTR pszFilePath = { nullptr };
					if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath))) {//사용자가 선택한 파일이나 폴더 하나(ㅈㄹ대경로 기준으로)
						result.push_back(ConvertToString(pszFilePath));
						CoTaskMemFree(pszFilePath);// 할당한 메모리를 해제
					}
					pItem->Release();
				}
			}
			pItemArray->Release();
		}
	}
	pFileOpen->Release();

	CoUninitialize();

	return result;
}
string Helper::SaveFileDialog()
{
	string savePath = "";
	filesystem::path defaultPath = SOLUTION_DIR;
	string filter = "json;txt;dat";

	nfdchar_t* outPath = nullptr;
	nfdresult_t result = NFD_SaveDialog(filter.c_str(), defaultPath.string().c_str(), &outPath);

	if (result == NFD_OKAY)
	{
		savePath = outPath;
		if (Helper::ContainsNonAscii(savePath)) {
			MSG_BOX("Folder Can't Be Open : Not English Path");
			savePath.clear();
		}
		else if (!Helper::IsPathInProjectFolder(filesystem::path(savePath).parent_path().string())) { //파일 부모 폴더 경로까지만
			MSG_BOX("Folders outside the project folder cannot be selected");
			savePath.clear();
		}
	}
	else if (result == NFD_ERROR)
	{
		MSG_BOX("Folder Open Error : CGUISystem");
	}

	if (outPath)
		free(outPath);

	return savePath;
}

ENGINE_DLL string Helper::SaveFileDialog(const string& fileName, const string& filter)
{
	string savePath = "";
	filesystem::path defaultFolder = SOLUTION_DIR;
	const wstring projectPath = std::filesystem::weakly_canonical(SOLUTION_DIR);


	nfdchar_t* outPath = nullptr;
	string strPath = ConvertToString(projectPath) + fileName;
	nfdresult_t result = NFD_SaveDialog(filter.c_str(), defaultFolder.string().c_str(), &outPath);

	if (result == NFD_OKAY)
	{
		savePath = outPath;
		if (Helper::ContainsNonAscii(savePath)) {
			MSG_BOX("Folder Can't Be Open : Not English Path");
			savePath.clear();
		}
		else if (!Helper::IsPathInProjectFolder(filesystem::path(savePath).parent_path().string())) { //파일 부모 폴더 경로까지만
			MSG_BOX("Folders outside the project folder cannot be selected");
			savePath.clear();
		}
	}
	else if (result == NFD_ERROR)
	{
		MSG_BOX("Folder Open Error : CGUISystem");
	}

	if (outPath)
		free(outPath);

	return savePath;
}

wstring Helper::ConvertToWideString(const string& str)
{
	if (str.empty()) return wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

string Helper::ConvertToString(const wstring& wstr)
{
	if (wstr.empty()) return string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

ENGINE_DLL string Helper::GetFileNameWithExtension(const string& filePath)
{
	return filesystem::path(filePath).filename().string();
}

ENGINE_DLL string Helper::GetFileNameWithOutExtension(const string& filePath)
{
	return filesystem::path(filePath).stem().string();
}

ENGINE_DLL string Helper::ToLower(const string& fileName)
{
	string result = fileName;
	transform(result.begin(), result.end(), result.begin(),
		[](unsigned char c) { return static_cast<char>(tolower(c)); });
	return result;
}

ENGINE_DLL string Helper::SaveFileDialogByWinAPI(const string& fileName, const string& filterStr)
{
	// 1. Windows API가 요구하는 형식으로 문자열을 준비합니다. (wide string)
	wchar_t filePathBuffer[MAX_PATH] = { 0, };
	wstring wideFileName = ConvertToWideString(fileName);
	wcscpy_s(filePathBuffer, MAX_PATH, wideFileName.c_str());

	wstring wideFilter;
	wstring defaultExtension;

	if (filterStr.empty())
	{
		wideFilter = L"JSON Files (*.json)\0*.json\0Data Files (*.dat)\0*.dat\0Model Files (*.model)\0*.model\0Material Files (*.mat)\0*.mat\Animation Files (*.anim)\0*.anim\0All Files (*.*)\0*.*\0";
		defaultExtension = L"json"; // 기본값일 경우 json을 기본 확장자로 설정
	}
	else
	{
		wstring wideExt = ConvertToWideString(filterStr);
		wstring pattern = L"*." + wideExt;
		wideFilter = pattern + L'\0' + pattern + L'\0' + L"All Files (*.*)\0*.*\0";
		defaultExtension = wideExt; // 인자로 받은 확장자를 기본 확장자로 설정
	}

	// 3. OPENFILENAMEW 구조체를 설정합니다.
	OPENFILENAMEW ofn = { 0, };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = filePathBuffer;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = wideFilter.c_str();
	ofn.lpstrDefExt = defaultExtension.c_str();
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

	// 4. Windows 파일 저장 대화상자를 호출합니다.
	if (GetSaveFileNameW(&ofn) == TRUE)
	{
		// 성공했다면, 사용자가 선택한 경로(filePathBuffer)를 std::string으로 변환하여 반환
		return Helper::ConvertToString(filePathBuffer);
	}

	// 취소했거나 실패했다면 빈 문자열 반환
	return "";
}

ENGINE_DLL HRESULT Helper::SaveTextureToDDs(ID3D11DeviceContext* pContext, const string& filePath, ID3D11ShaderResourceView* pSRV)
{
	pContext->GenerateMips(pSRV);

	ID3D11Resource* pResource = { nullptr };
	pSRV->GetResource(&pResource);

	ID3D11Texture2D* pTexture2D;
	HRESULT hr_cast = pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTexture2D);

	if (SUCCEEDED(hr_cast))
	{
		D3D11_TEXTURE2D_DESC desc;
		pTexture2D->Release();
	}

	HRESULT hr = SaveDDSTextureToFile(pContext, pResource, ConvertToWideString(filePath).c_str());
	if (pResource)
	{
		pResource->Release();
	}

	return hr;
}

ENGINE_DLL HRESULT Helper::SaveTextureToDDs(ID3D11DeviceContext* pContext, const wstring& filePath, ID3D11ShaderResourceView* pSRV)
{
	pContext->GenerateMips(pSRV);

	ID3D11Resource* pResource = { nullptr };
	pSRV->GetResource(&pResource);

	ID3D11Texture2D* pTexture2D;
	HRESULT hr_cast = pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTexture2D);

	if (SUCCEEDED(hr_cast))
	{
		D3D11_TEXTURE2D_DESC desc;
		pTexture2D->Release();
	}

	HRESULT hr = SaveDDSTextureToFile(pContext, pResource, filePath.c_str());
	if (pResource)
	{
		pResource->Release();
	}
	return hr;
}
static mt19937& RNG()
{
	static mt19937 rng(random_device{}());
	return rng;
}

ENGINE_DLL _int Helper::Get_Random_Int(_int min, _int max)
{
	uniform_int_distribution<_int> dist(min, max);
	return dist(RNG());
}

ENGINE_DLL _float Helper::Get_Random_Float(_float min, _float max)
{
	uniform_real_distribution<_float> dist(min, max);
	return dist(RNG());

}

ENGINE_DLL _bool Helper::IsUnderDirectory(const filesystem::path& file, const filesystem::path& dir)
{
	error_code ec;

	auto f = std::filesystem::weakly_canonical(file, ec);
	if (ec) return false;

	auto d = std::filesystem::weakly_canonical(dir, ec);
	if (ec) return false;

	auto fit = f.begin();
	for (auto dit = d.begin(); dit != d.end(); ++dit, ++fit)
	{
		if (fit == f.end() || *fit != *dit)
			return false;
	}
	return true;
}
// -------------------------------------------------------------------------------------------------

ENGINE_DLL bool Helper::DrawEaseComboPopup(EaseType& ioValue, EaseType shownValue)
{
	bool changed = false;
	bool previewShown = false;

	auto ShowPreview = [&](EaseType v)
		{
			if (previewShown) return;
			previewShown = true;

			ImVec2 itemMin = ImGui::GetItemRectMin();
			ImVec2 itemMax = ImGui::GetItemRectMax();

			const ImGuiStyle& style = ImGui::GetStyle();
			const ImVec2 display = ImGui::GetIO().DisplaySize;

			const float graphW = 260.f;
			const float graphH = 110.f;

			const float textH = ImGui::GetTextLineHeightWithSpacing();
			const float tooltipW = graphW + style.WindowPadding.x * 2.f;
			const float tooltipH = graphH + textH + style.ItemSpacing.y + style.WindowPadding.y * 2.f;

			float x = itemMax.x + 12.f;
			float y = itemMin.y;

			if (x + tooltipW > display.x - 4.f) x = itemMin.x - 12.f - tooltipW;
			if (y + tooltipH > display.y - 4.f) y = display.y - 4.f - tooltipH;
			if (y < 4.f) y = 4.f;

			ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always);
			ImGui::SetNextWindowBgAlpha(0.96f);

			ImGuiWindowFlags wf =
				ImGuiWindowFlags_Tooltip |
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoFocusOnAppearing |
				ImGuiWindowFlags_NoNav;

			if (ImGui::Begin("##EaseHoverPreview", nullptr, wf))
			{
				ImGui::TextDisabled("%s", Helper::EnumLabel(v));

				ImGui::PushID((int)v);
				DrawEaseGraph(v, ImVec2(graphW, graphH), "##ease_hover_graph");
				ImGui::PopID();

				ImGui::End();
			}
		};

	auto Pick = [&](EaseType v)
		{
			const bool selected = (shownValue == v);

			if (ImGui::Selectable(Helper::EnumLabel(v), selected))
			{
				ioValue = v;
				changed = true;
			}

			if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
				ShowPreview(v);

			if (selected) ImGui::SetItemDefaultFocus();
		};

	Pick(EaseType::None);

	ImGui::SeparatorText("A. Stable");
	Pick(EaseType::InOutSine);
	Pick(EaseType::OutCubic);
	Pick(EaseType::InOutCubic);
	Pick(EaseType::OutSine);
	Pick(EaseType::InOutQuad);

	ImGui::SeparatorText("B. Ease In");
	Pick(EaseType::InSine);
	Pick(EaseType::InCubic);
	Pick(EaseType::InQuad);
	Pick(EaseType::InCirc);

	ImGui::SeparatorText("C. Settle / Stop");
	Pick(EaseType::InOutCirc);
	Pick(EaseType::OutCirc);
	Pick(EaseType::OutQuad);

	ImGui::SeparatorText("D. Strong");
	Pick(EaseType::InQuart);
	Pick(EaseType::InQuint);
	Pick(EaseType::InOutQuart);
	Pick(EaseType::OutQuart);
	Pick(EaseType::InOutQuint);
	Pick(EaseType::OutQuint);

	ImGui::SeparatorText("E. Extreme");
	Pick(EaseType::InOutExpo);
	Pick(EaseType::OutExpo);
	Pick(EaseType::InExpo);

	ImGui::SeparatorText("F. Overshoot");
	Pick(EaseType::OutBack);
	Pick(EaseType::InOutBack);
	Pick(EaseType::InBack);

	ImGui::SeparatorText("G. Special");
	Pick(EaseType::OutElastic);
	Pick(EaseType::InOutElastic);
	Pick(EaseType::InElastic);
	Pick(EaseType::OutBounce);
	Pick(EaseType::InOutBounce);
	Pick(EaseType::InBounce);

	return changed;
}

ENGINE_DLL void Helper::DrawEaseGraph(EaseType ease, ImVec2 size, const char* id)
{
	if (size.x <= 10.f) size.x = 360.f;
	if (size.y <= 10.f) size.y = 180.f;

	ImDrawList* dl = ImGui::GetWindowDrawList();
	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 p1(p0.x + size.x, p0.y + size.y);

	ImGui::InvisibleButton(id, size);

	ImU32 colBg = ImGui::GetColorU32(ImGuiCol_FrameBg);
	ImU32 colBorder = ImGui::GetColorU32(ImGuiCol_Border);
	ImU32 colGrid = ImGui::GetColorU32(ImGuiCol_TextDisabled);
	ImU32 colLinear = ImGui::GetColorU32(ImGuiCol_TextDisabled);
	ImU32 colCurve = ImGui::GetColorU32(ImGuiCol_Text);
	ImU32 colWarn = ImGui::GetColorU32(ImGuiCol_ButtonActive);

	dl->AddRectFilled(p0, p1, colBg, 6.f);
	dl->AddRect(p0, p1, colBorder, 6.f);

	const int samples = 160;
	float minY = 1e9f;
	float maxY = -1e9f;

	for (int i = 0; i <= samples; ++i)
	{
		float u = (float)i / (float)samples;
		float y = Math::ApplyEase(ease, u);
		minY = min(minY, y);
		maxY = max(maxY, y);
	}

	float lo = minY;
	float hi = maxY;

	if (lo > 0.f) lo = 0.f;
	if (hi < 1.f) hi = 1.f;

	float pad = (hi - lo) * 0.08f;
	if (pad < 0.02f) pad = 0.02f;

	lo -= pad;
	hi += pad;

	lo = max(lo, -1.0f);
	hi = min(hi, 2.0f);

	auto ToScreen = [&](float u, float y) -> ImVec2
		{
			float y01 = (y - lo) / (hi - lo);
			y01 = clamp(y01, 0.f, 1.f);

			float x = p0.x + u * size.x;
			float ypix = p1.y - y01 * size.y;
			return ImVec2(x, ypix);
		};

	const int gridN = 4;
	for (int i = 1; i < gridN; ++i)
	{
		float t = (float)i / (float)gridN;
		float x = p0.x + t * size.x;
		float y = p0.y + t * size.y;

		dl->AddLine(ImVec2(x, p0.y), ImVec2(x, p1.y), colGrid, 1.f);
		dl->AddLine(ImVec2(p0.x, y), ImVec2(p1.x, y), colGrid, 1.f);
	}

	dl->AddLine(ToScreen(0.f, 0.f), ToScreen(1.f, 1.f), colLinear, 1.5f);

	const bool overshoot = (minY < 0.f) || (maxY > 1.f);

	ImVec2 prev = ToScreen(0.f, Math::ApplyEase(ease, 0.f));
	for (int i = 1; i <= samples; ++i)
	{
		float u = (float)i / (float)samples;
		float y = Math::ApplyEase(ease, u);
		ImVec2 cur = ToScreen(u, y);
		dl->AddLine(prev, cur, overshoot ? colWarn : colCurve, 2.0f);
		prev = cur;
	}

	ImVec2 labelPos(p0.x + 10.f, p0.y + 8.f);
	dl->AddText(labelPos, ImGui::GetColorU32(ImGuiCol_Text), Helper::EnumLabel(ease));

	char rangeBuf[128];
	sprintf_s(rangeBuf, "y range: %.2f .. %.2f", minY, maxY);
	dl->AddText(ImVec2(labelPos.x, labelPos.y + 18.f), ImGui::GetColorU32(ImGuiCol_TextDisabled), rangeBuf);

	if (overshoot)
		dl->AddText(ImVec2(labelPos.x, labelPos.y + 36.f), colWarn, "Overshoot");
}

ENGINE_DLL bool Helper::DrawEaseCombo(const char* id, EaseType& ioValue, EaseType shownValue, float width)
{
	ImGui::SetNextItemWidth(width);

	if (!ImGui::BeginCombo(id, Helper::EnumLabel(shownValue)))
		return false;

	const bool changed = DrawEaseComboPopup(ioValue, shownValue);

	ImGui::EndCombo();
	return changed;
}

ENGINE_DLL bool Helper::DrawEaseCombo(const char* id, EaseType& ioValue, float width)
{
	return DrawEaseCombo(id, ioValue, ioValue, width);
}