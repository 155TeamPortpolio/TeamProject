#include "ModelHelper.h"

HRESULT ModelHelper::Add_ModelPathFromDirectory(vector<string>& directoryPath, string path)
{
	for (const auto& entry : filesystem::recursive_directory_iterator(filesystem::path(path))) {
		if (!entry.is_regular_file())
			continue;

		string ext = entry.path().extension().string();

		if (ext == ".json") {
			directoryPath.push_back(entry.path().string());
		}
	}

	return S_OK;
}
