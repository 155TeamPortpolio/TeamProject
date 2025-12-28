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
#include "HUD.h"

namespace fs = filesystem;
using namespace Helper;

namespace
{   
	void RegisterUIProtos(const string& levelKey)
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
			{"HUD",             &CHUD::Create               },
		};

		for (const Entry& entry : entries)
		{
			const string protoTag = string("Proto_GameObject_") + entry.typeTag;
			CGameInstance::GetInstance()->Get_PrototypeMgr()->Add_ProtoType(levelKey, protoTag, entry.createFunc());
		}
	}

	// 확장자(ext) 를 소문자로 변환 {".png", ".dds", ".json"}
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
	// 허용된 확장자면 등록 아니면 무시
	bool ExtAllowed(const fs::path& path, const unordered_set<string>& allowed)
	{
		if (allowed.empty()) return true;
		const string ext = ToLower(path.extension().string());
		return allowed.find(ext) != allowed.end();
	}
	// rootDir 폴더를 재귀 스캔해서, exts 확장자만 골라 RES에 등록, 
	// 등록 규칙: key = 파일명(filename), value = 파일경로(generic_string)
	void ScanDirectory(const string& rootDir, const vector<string>& exts)
	{
		const fs::path root = fs::path(rootDir);
		const auto allowed = BuildExtSet(exts);

		for (auto& entry : fs::recursive_directory_iterator(root))
		{
			if (!entry.is_regular_file()) continue;

			const fs::path path = entry.path();
			if (!ExtAllowed(path, allowed)) continue;
			CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(path.filename().string(), path.generic_string());
		}
	}

	void RegisterFonts(const string& fontDir)
	{
		const fs::path root = fs::path(fontDir);
		const auto allowed = BuildExtSet({".spritefont"});

		for (const auto& entry : fs::recursive_directory_iterator(root))
		{
			if (!entry.is_regular_file()) continue;

			const fs::path filePath = entry.path();
			if (!ExtAllowed(filePath, allowed)) continue;

			CGameInstance::GetInstance()->Get_FontSystem()->Add_Font(filePath.filename().string(), 
				ConvertToWideString(filePath.generic_string()).c_str());
		}
	}
}

void UILoader::RegisterUI(const string& levelKey)
{
	static unordered_set<string> registeredLevels;
	if (registeredLevels.find(levelKey) != registeredLevels.end()) return;
	registeredLevels.insert(levelKey);

	const string uiRoot = "../Bin/Resources/UI";

	ScanDirectory(uiRoot + "/Image", { ".dds", ".png" });
	ScanDirectory(uiRoot + "/Data",  { ".json" });
	RegisterFonts(uiRoot + "/Font");

	RegisterUIProtos(levelKey);
}