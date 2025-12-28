#include "pch.h"
#include "CamLoader.h"
#include "CamDirector.h"

namespace fs = filesystem;

namespace
{
	void LoadFolder(const fs::path& folder)
	{
		auto& director = *CCamDirector::GetInstance();

		if (!fs::exists(folder) || !fs::is_directory(folder)) return;

		for (const auto& it : fs::recursive_directory_iterator(folder))
		{
			if (!it.is_regular_file()) continue;

			const auto& path = it.path();
			if (path.extension() != ".cam") continue;

			fs::path rel = fs::relative(path, folder);
			rel.replace_extension();

			const string key = rel.generic_string();
			director.Register(key, path);
		}
	}
}

void CamLoader::Load()
{
	LoadFolder("../../Resources/Data/Camera");
}