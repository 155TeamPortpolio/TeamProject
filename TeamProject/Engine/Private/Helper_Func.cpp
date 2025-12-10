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

	return false;
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