#pragma once

namespace CamLoader
{
	void LoadFolder(const filesystem::path& folder)
	{
		auto& director = *CCamDirector::GetInstance();

		assert(filesystem::exists(folder));
		assert(filesystem::is_directory(folder));

		for (const auto& it : filesystem::recursive_directory_iterator(folder))
		{
			if (!it.is_regular_file()) continue;

			const auto& path = it.path();
			if (path.extension() != ".cam") continue;

			filesystem::path rel = filesystem::relative(path, folder);
			rel.replace_extension();

			const string key = rel.generic_string();
			director.Register(key, path);
		}
	}

	void Load()
	{
		LoadFolder("../../Resources/Data/Camera");
	}
}