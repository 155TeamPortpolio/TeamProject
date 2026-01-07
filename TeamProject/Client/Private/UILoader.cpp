#include "pch.h"
#include "UILoader.h"
#include "UIDirector.h"
#include "Helper_Func.h"
#include "GameInstance.h"

#include "ButtonUI.h"
#include "CanvasPanel.h" 
#include "GaugeUI.h"
#include "ImageUI.h"
#include "SpriteAnimationUI.h"
#include "TextUI.h"
#include "UVAnimationUI.h"
#include "MaskUI.h"

#include "UI_Logo.h"
#include "UI_Loading.h"
#include "UI_BattleHUD.h"

namespace fs = filesystem;
using namespace Helper;

namespace
{
	unordered_set<string> BuildExtSet(const vector<string>& exts)
	{
		unordered_set<string> out;
		out.reserve(exts.size());

		for (auto ext : exts)
		{
			if (!ext.empty() && ext[0] != '.')
				ext = "." + ext;
			out.insert(ToLower(ext));
		}
		return out;
	}

	bool ExtAllowed(const fs::path& path, const unordered_set<string>& allowed)
	{
		if (allowed.empty()) return true;
		const string ext = ToLower(path.extension().string());
		return allowed.find(ext) != allowed.end();
	}
	// skip_permission_denied: 권한 문제는 터지지말고 건너뛰기
	// increment(errorCode): 다음 항목으로 넘어갈 때 에러가 나면 예외 대신 errorCode로 받음(안터지고 계속 진행가능)
	void ScanDirectory(const string& rootDir, const vector<string>& exts)
	{
		const fs::path root = fs::path(rootDir);
		if (!fs::exists(root) || !fs::is_directory(root)) return;

		const auto allowed = BuildExtSet(exts);

		error_code errorCode{};
		fs::recursive_directory_iterator it(root, fs::directory_options::skip_permission_denied, errorCode);
		const fs::recursive_directory_iterator end{};

		for (; it != end; it.increment(errorCode))
		{
			if (errorCode) { errorCode.clear(); continue; }

			const auto& entry = *it;
			if (!entry.is_regular_file(errorCode)) { errorCode.clear(); continue; }

			const fs::path path = entry.path();
			if (!ExtAllowed(path, allowed)) continue;

			CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(path.filename().string(), path.generic_string());
		}
	}

	void RegisterFonts(const string& fontDir)
	{
		const fs::path root = fs::path(fontDir);
		if (!fs::exists(root) || !fs::is_directory(root)) return;

		const auto allowed = BuildExtSet({".spritefont"});

		error_code errorCode{};
		fs::recursive_directory_iterator it(root, fs::directory_options::skip_permission_denied, errorCode);
		const fs::recursive_directory_iterator end{};

		for (; it != end; it.increment(errorCode))
		{
			if (errorCode) { errorCode.clear(); continue; }
			const auto& entry = *it;

			if (!entry.is_regular_file(errorCode)) { errorCode.clear(); continue; }

			const fs::path filePath = entry.path();
			if (!ExtAllowed(filePath, allowed)) continue;

			CGameInstance::GetInstance()->Get_FontSystem()->Add_Font( filePath.filename().string(),
				ConvertToWideString(filePath.generic_string()).c_str());
		}
	}
}

void UILoader::Add_ResourcePath()
{
	const string uiRoot = "../Bin/Resources/UI";
	const string jsonRoot = "../../Resources/Data/UI";

	ScanDirectory(uiRoot + "/Image", {".dds", ".png"});
	ScanDirectory(jsonRoot, {".json"});
	RegisterFonts(uiRoot + "/Font");
}

void UILoader::Add_Prototype(const string& levelKey)
{
	struct Entry { const char* typeTag; CGameObject* (*createFunc)(); };

	static const Entry entries[] =
	{
		{"Button",          &CButtonUI::Create          },
		{"CanvasPanel",     &CCanvasPanel::Create       },
		{"Gauge",           &CGaugeUI::Create           },
		{"Image",           &CImageUI::Create           },
		{"SpriteAnimation", &CSpriteAnimationUI::Create },
		{"Text",            &CTextUI::Create            },
		{"UVAnimation",     &CUVAnimationUI::Create     },
		{"Mask",			&CMaskUI::Create			},
		{"Logo",			&CUI_Logo::Create			},
		{"Loading",			&CUI_Loading::Create		},
		{"BattleHUD",		&CUI_BattleHUD::Create		},
	};

	for (const Entry& entry : entries)
	{
		const string protoTag = string("Proto_GameObject_") + entry.typeTag;
		CGameInstance::GetInstance()->Get_PrototypeMgr()->Add_ProtoType(levelKey, protoTag, entry.createFunc());
	}
}