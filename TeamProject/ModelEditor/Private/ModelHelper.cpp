#include "ModelHelper.h"
#include "Helper_Func.h"
#include <filesystem>
#include <system_error>

HRESULT ModelHelper::Add_ModelPathFromDirectory(std::vector<std::string>& directoryPath, std::string path)
{
	namespace fs = std::filesystem;
	error_code ec;

	// 폴더 없거나 디렉터리 아니면 조용히 스킵
	if (!fs::exists(path, ec) || !fs::is_directory(path, ec))
		return S_OK;

	// 권한/접근 문제도 예외 대신 ec로 처리
	fs::recursive_directory_iterator it(fs::path(path), fs::directory_options::skip_permission_denied, ec);
	fs::recursive_directory_iterator end;

	for (; it != end; it.increment(ec))
	{
		if (ec) { ec.clear(); continue; } // 해당 엔트리에서 오류나면 넘어감

		const auto& entry = *it;
		if (!entry.is_regular_file(ec)) continue;
		string path =Helper::ToLower(entry.path().string());
		string LOD = "lod";
		string proxy = "proxy";
		if (path.find(LOD) != string::npos) continue;
		if (path.find(proxy) != string::npos) continue;
		if (entry.path().extension() != ".json")continue;

		directoryPath.push_back(entry.path().string());
	}

	return S_OK;
}
