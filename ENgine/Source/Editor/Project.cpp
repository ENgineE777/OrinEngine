#include "Editor.h"
#include "Support/Sprite.h"
#include "shlobj_core.h"
#include "Support/Perforce.h"

namespace Oak
{
	bool Project::CanRun()
	{
		if (startScene.empty())
		{
			MESSAGE_BOX("Can't start", "Please define a start scene");

			return false; 
		}

		return true;
	}

	bool Project::Load(const char* fileName)
	{
		Reset();

		projectFullName = fileName;
		StringUtils::GetPath(projectFullName.c_str(), projectPath);
		StringUtils::FixSlashes(projectPath);

		root.SetRootPath(projectPath);
		root.assets.LoadAssets();

		if (!root.scripts.CompileProjectCode(false))
		{
			projectFullName = "";
			return false;
		}

		JsonReader reader;

		if (reader.ParseFile(projectFullName.c_str()))
		{
			reader.Read("start_scene", startScene);
			reader.Read("export_dir", exportDir);

			reader.Read("iconPath", iconPath);
			icon = root.render.LoadTexture(iconPath.c_str(), _FL_);

			reader.Read("iconSmallPath", iconSmallPath);
			iconSmall = root.render.LoadTexture(iconSmallPath.c_str(), _FL_);

			reader.Read("projectName", projectName);
			reader.Read("alignRect", editor.gizmo.alignRect);
			reader.Read("useAlignRect", editor.gizmo.useAlignRect);

			float pixelsPerUnit = 50.0f;
			float pixelsHeight = 1080.0f;

			reader.Read("pixelsPerUnit", pixelsPerUnit);
			reader.Read("pixelsHeight", pixelsHeight);

			Sprite::SetData(pixelsHeight, pixelsPerUnit);

			reader.Read("hideCursor", hideCursor);

			eastl::string assetPath;

			while (reader.EnterBlock("openedAssets"))
			{
				reader.Read("path", assetPath);
				
				editor.openedAssets.push_back(root.assets.GetAssetRef<AssetRef>(assetPath));

				reader.LeaveBlock();
			}
			
			reader.Read("edited_asset", assetPath);

			auto asset = root.assets.GetAssetRef<AssetRef>(assetPath);

			if (asset)
			{
				editor.SelectEditAsset(asset);
			}

			while (reader.EnterBlock("layers"))
			{
				layers.push_back(Layer());
				Layer& layer = layers.back();

				reader.Read("name", layer.name);
				reader.Read("state", (int&)layer.state);

				reader.LeaveBlock();
			}
		}

		eastl::string p4FilePath = projectPath + eastl::string("P4Config");
		if (reader.ParseFile(p4FilePath.c_str()))
		{
			reader.Read("p4URL", p4URL);
			reader.Read("p4Workspace", p4Workspace);
			reader.Read("p4User", p4User);

			Perforce::SetConfig(p4URL.c_str(), p4Workspace.c_str(), p4User.c_str());
		}

		return true;
	}

	void Project::Save(const char* fileName)
	{
		bool needSetRoot = projectFullName[0] ? false : true;

		projectFullName = fileName;
		StringUtils::GetPath(projectFullName.c_str(), projectPath);

		Save();

		if (needSetRoot)
		{
			root.SetRootPath(projectPath);
			root.assets.LoadAssets();
		}
	}

	void Project::Save()
	{
		JsonWriter writer;
		writer.Start(projectFullName.c_str());

		writer.Write("export_dir", exportDir.c_str());
		writer.Write("iconPath", iconPath.c_str());
		writer.Write("iconSmallPath", iconSmallPath.c_str());

		if (projectName.empty())
		{
			char name[128];
			StringUtils::GetFileName(projectFullName.c_str(), name);
			StringUtils::RemoveExtension(name);

			projectName = name;
		}

		writer.Write("projectName", projectName.c_str());

		writer.Write("start_scene", startScene);

		writer.Write("alignRect", editor.gizmo.alignRect);
		writer.Write("useAlignRect", editor.gizmo.useAlignRect);

		writer.Write("pixelsPerUnit", Sprite::ToPixels(1.0f));
		writer.Write("pixelsHeight", Sprite::GetPixelsHeight());

		writer.Write("hideCursor", hideCursor);

		writer.StartArray("openedAssets");

		for (auto& asset : editor.openedAssets)
		{
			writer.StartBlock(nullptr);

			writer.Write("path", asset->GetPath().c_str());

			writer.FinishBlock();
		}

		writer.FinishArray();

		writer.Write("edited_asset", editor.selectedEditAsset ? editor.selectedEditAsset->GetPath().c_str() : "");

		writer.StartArray("layers");

		for (auto& layer : layers)
		{
			writer.StartBlock(nullptr);

			writer.Write("name", layer.name.c_str());
			writer.Write("state", layer.state);

			writer.FinishBlock();
		}

		writer.FinishArray();

		eastl::string p4FilePath = projectPath + eastl::string("P4Config");
		writer.Start(p4FilePath.c_str());

		writer.Write("p4URL", p4URL);
		writer.Write("p4Workspace", p4Workspace);
		writer.Write("p4User", p4User);
	}

	void Project::SetStartScene(const eastl::string& path)
	{
		char name[128];
		StringUtils::GetFileName(path.c_str(), name);

		StringUtils::RemoveExtension(name);
		startScene = name;
	}

	bool Project::IsStartScene(const eastl::string& name)
	{
		char name_str[128];
		StringUtils::Copy(name_str, 128, name.c_str());
		StringUtils::RemoveExtension(name_str);

		return StringUtils::IsEqual(startScene.c_str(), name_str);
	}

	int Project::FindLayer(const char* layerName)
	{
		auto iter = eastl::find_if(layers.begin(), layers.end(), [layerName](const Layer& layer) { return StringUtils::IsEqual(layer.name.c_str(), layerName); });

		
		return iter == layers.end() ? -1 : (int)(iter - layers.begin());
	}

	void Project::AddLayer(const char* name)
	{
		if (!name[0])
		{
			return;
		}

		if (FindLayer(name) != -1)
		{
			return;
		}

		layers.push_back(Layer());
		Layer& layer = layers.back();

		layer.name = name;
	}

	void Project::DeleteLayer(const char* name)
	{
		int index = FindLayer(name);

		if (index != -1)
		{
			layers.erase(layers.begin() + index);
		}
	}

	bool Project::LayerHiden(const char* name)
	{
		int index = FindLayer(name);

		if (index == -1)
		{
			return false;
		}

		return layers[index].state == Layer::Invisible;
	}

	bool Project::LayerSelectable(const char* name)
	{
		int index = FindLayer(name);

		if (index == -1)
		{
			return true;
		}

		return layers[index].state == Layer::Normal;
	}


	void Project::Reset()
	{
		icon.ReleaseRef();
		iconSmall.ReleaseRef();

		Sprite::SetData(1080.0f, 50.0f);

		editor.SelectEditAsset(AssetRef());

		root.assets.Clear();

		projectFullName.clear();
		exportDir.clear();
		startScene = -1;
	}

	void Project::SelectExportDir()
	{
		BROWSEINFOA bi = { 0 };

		bi.lpszTitle = "Browse for Folder";

		LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);

		if (pidl != NULL)
		{
			CHAR tszPath[MAX_PATH] = "\0";

			if (SHGetPathFromIDListA(pidl, tszPath) == TRUE)
			{
				exportDir = tszPath;
			}

			CoTaskMemFree(pidl);
		}
	}

	void Project::CpyFolder(const char* src, const char* dest)
	{
		root.files.CpyFolder((eastl::string(applicationDir) + src).c_str(), (exportDir + (dest != nullptr ? dest : src)).c_str());
	}

	void Project::CpyFile(const char* src, const char* dest)
	{
		root.files.CpyFile((eastl::string(applicationDir) + src).c_str(), (exportDir + (dest != nullptr ? dest : src)).c_str());
	}

	void Project::UpdateExeStrings(HANDLE resource)
	{
		eastl::wstring strings[16];

		StringUtils::Utf8toUtf16(strings[7], projectName.c_str());
		StringUtils::Utf8toUtf16(strings[13], "Oak");

		char buffer[256];
		int index = 0;

		for (auto& str : strings)
		{
			uint16_t sz = (uint16_t)str.size();
			memcpy(&buffer[index], &sz, 2);
			index += 2;

			for (int j = 0; j < str.size(); j++)
			{
				uint16_t chr = str[j];
				memcpy(&buffer[index], &chr, 2);
				index += 2;
			}
		}

		if (!UpdateResourceA(resource, MAKEINTRESOURCEA(6), MAKEINTRESOURCEA(7), 1033, (LPVOID)buffer, index))
		{
			auto err = GetLastError();
			root.Log("Export", "Failed to update string in output exe with error %u", err);
		}
	}

	void Project::UpdateIconsSet(HANDLE resource, const char* path, bool smallIcon, int idOffest)
	{
		int sizes[] = { 16, 24, 32, 48, 64, 72, 96, 128, 256 };

		char icoPath[512];
		StringUtils::Printf(icoPath, 512, "%s%s", root.GetPath(Root::Path::Assets), path);
		StringUtils::RemoveExtension(icoPath);

		for (int i = 0; i < sizeof(sizes) / sizeof(int); i++)
		{
			if (system(StringUtils::PrintTemp("%s/ENgine/Tools/nconvert/nconvert.exe -resize %i %i -overwrite -out ico \"%s%s\"", applicationDir, sizes[i], sizes[i], root.GetPath(Root::Path::Assets), path)) == 0)
			{
				FileInMemory ico;

				if (ico.Load(StringUtils::PrintTemp("%s.ico", icoPath)))
				{
					uint8_t* ptr = ico.GetPtr();
					ptr += 22;

					if (!UpdateResourceA(resource, MAKEINTRESOURCEA(3), MAKEINTRESOURCEA(i + 1 + idOffest), 1033, (LPVOID)ptr, ico.GetSize() - 22))
					{
						auto err = GetLastError();
						root.Log("Export", "Failed to update icon set in output exe with error %u", err);
					}
				}
			}

			if (smallIcon && i == 4)
			{
				break;
			}
		}

		DeleteFileA(StringUtils::PrintTemp("%s.ico", icoPath));
	}

	void Project::Export()
	{
		if (projectFullName.empty())
		{
			return;
		}

		if (exportDir.empty())
		{
			SelectExportDir();

			if (exportDir.empty())
			{
				return;
			}
		}

		Save();

		GetCurrentDirectoryA(512, applicationDir);

		root.files.DeleteFolder(exportDir.c_str());

		root.files.CpyFolder(projectPath, (exportDir + "/project").c_str());

		root.files.DeleteFolder((exportDir + "/project/_Code").c_str());

		{
			char project_file_name[512];
			StringUtils::GetFileName(projectFullName.c_str(), project_file_name);

			rename((exportDir + "/project/" + project_file_name).c_str(), (exportDir + "/project/project.pra").c_str());
		}

		CpyFolder("/ENgine/Controls");
		CpyFolder("/ENgine/Shaders/PC");

		CpyFolder("/ENgine/ExportBinaries", "");

		CpyFile("/eastl.dll");
		CpyFile("/fmod.dll");
		CpyFile("/fmodL.dll");
		CpyFile("/Oak.dll");
		CpyFile("/PhysX_64.dll");
		CpyFile("/PhysXCommon_64.dll");
		CpyFile("/PhysXCooking_64.dll");
		CpyFile("/PhysXFoundation_64.dll");

		CpyFile("/ENgine/helvetica");
		CpyFile("/ENgine/low2hi.dat");
		CpyFile("/ENgine/OpenSans-Regular.ttf");

		auto resource = BeginUpdateResourceA((exportDir + "/Oak.exe").c_str(), FALSE);
		if (resource != NULL)
		{
			UpdateExeStrings(resource);

			if (icon.Get())
			{
				UpdateIconsSet(resource, iconPath.c_str(), false, 0);

				UpdateIconsSet(resource, iconSmall.Get() ? iconSmallPath.c_str() : iconPath.c_str(), true, 9);
			}

			EndUpdateResourceA(resource, FALSE);
		}

		rename((exportDir + "/Oak.exe").c_str(), (exportDir + "/" + projectName + ".exe").c_str());

		MESSAGE_BOX("Export finished", (eastl::string("Resources of project were exported to folder:\n") + exportDir).c_str());
	}
}