
#include "Root/Root.h"

#include "Root/Scenes/SceneEntity.h"
#include "Editor/Editor.h"

#ifdef ORIN_EDITOR
#include <filesystem>
#endif

namespace Orin
{
	#ifdef PLATFORM_WIN
	void register_code(eastl::vector<ClassFactorySceneEntity*> & Decls);
	void recreate_entites(eastl::vector<SceneEntity*> & entities);
	void unregister_code(eastl::vector<ClassFactorySceneEntity*> & Decls);

	std::string exec(const char* cmd)
	{
		std::shared_ptr<FILE> pipe(_popen(cmd, "r"), _pclose);
		if (!pipe) return "ERROR";
		char buffer[128];
		std::string result = "";
		while (!feof(pipe.get())) {
			if (fgets(buffer, 128, pipe.get()) != NULL)
				result += buffer;
		}
		return result;
	}

	bool Scripts::CompileProjectCode(bool forceCompile)
	{
		if (forceCompile || !std::filesystem::exists(StringUtils::PrintTemp("%sgameplay_%s.dll", root.GetPath(Root::Path::Binaries), configName.c_str())))
		{
			if (system(StringUtils::PrintTemp("VcProgGen.exe %s", root.GetPath(Root::Path::Project))) != 0)
			{
				MESSAGE_BOX("Can't compile script", "Can't generate vc proj");
			}
			else
			{
				char* programFiles = nullptr;
				size_t sz = 0;

				eastl::string vsPath;

				if (_dupenv_s(&programFiles, &sz, "ProgramFiles(x86)") == 0 && programFiles != nullptr)
				{
					auto res = exec(StringUtils::PrintTemp("\"%s/Microsoft Visual Studio/Installer/vswhere.exe\" -format json", programFiles));

					res.erase(res.begin(), res.begin() + 4);
					res.erase(res.begin() + res.size() - 2, res.begin() + res.size() - 1);

					JsonReader reader;
				
					if (reader.ParseString(res.c_str()))
					{
						bool isComplete = false;
						bool isLaunchable = false;;

						reader.Read("isComplete", isComplete);
						reader.Read("isLaunchable", isLaunchable);
						
						if (!isComplete || !isLaunchable)
						{
							MESSAGE_BOX("Can't compile script",  "Visual Studio was found but can't be launched. Please reinstall Visual Studio 2019");
						}
						else
						{
							reader.EnterBlock("catalog");

							eastl::string lineVersion;
							reader.Read("productLineVersion", lineVersion);

							reader.LeaveBlock();

							reader.Read("productPath", vsPath);

							DeleteFileA("errors.txt");

							if (system(StringUtils::PrintTemp("\"%s\" %s/gameplay.sln /out errors.txt /Build %s", vsPath.c_str(), root.GetPath(Root::Path::VcProj), configName.c_str())) != 0)
							{
								FileInMemory errors;

								if (errors.Load("errors.txt"))
								{
									eastl::string str = (const char*)errors.GetPtr();
									MESSAGE_BOX("Can't compile script", str.c_str());
								}
							}
						}
					}
					else
					{
						MESSAGE_BOX("Can't compile script", "Visual Studio was found but can't be launched. Please reinstall Visual Studio");
					}

					free(programFiles);
				}
				else
				{
					MESSAGE_BOX("Can't compile script", "Visual Studio was not found. Please install Visual Studio");
				}
			}
		}

		return CheckGamePlayDll();
	}

	void Scripts::LoadGamePlayDll(const char* path)
	{
		HMODULE newModule = LoadLibraryA(path);

		if (newModule)
		{
			AssetRef selectedEditAsset;

			if (Module && allowDynamicReload)
			{
				selectedEditAsset = editor.selectedEditAsset;
				editor.SelectEditAsset(AssetRef());

				void* Procs = GetProcAddress((HMODULE)Module, "unregister_code");
				auto code_ptr = (decltype(unregister_code)*)Procs;

				code_ptr(ClassFactorySceneEntity::Decls());
			}

			{
				void* Procs = GetProcAddress((HMODULE)newModule, "register_code");
				auto code_ptr = (decltype(register_code)*)Procs;

				code_ptr(ClassFactorySceneEntity::Decls());

				// We must reinit GroupedDecls because it contains dead pointers from old DLL
				ClassFactorySceneEntity::GropedDecls() = ClassFactorySceneEntity::Decls();

				ClassFactorySceneEntity::Sort();
				ClassFactorySceneEntity::SortGrouped();
			}

			if (Module && allowDynamicReload)
			{
				root.render.InvalidateNonEngineTechiques();

				{
					void* Procs = GetProcAddress(newModule, "recreate_entites");
					auto code_ptr = (decltype(recreate_entites)*)Procs;

					eastl::vector<AssetScene*> scenes;

					root.assets.GetAssetsByType<AssetScene>(scenes);

					for (auto* scene : scenes)
					{
						code_ptr((eastl::vector<SceneEntity*>&)scene->GetScene()->GetEntities());
					}
				}

				FreeLibrary((HMODULE)Module);

				char tmpname[256];
				StringUtils::Printf(tmpname, 256, "%sGameplay_%s.rcpp%i.dll", root.GetPath(Root::Path::Binaries), configName.c_str(), 1 - pingPong);
				DeleteFileA(tmpname);
			}

			Module = newModule;

			if (selectedEditAsset)
			{
				editor.SelectEditAsset(selectedEditAsset);
			}
		}
	}

	bool Scripts::CheckGamePlayDll()
	{
		HANDLE hDLLFile = CreateFileA(StringUtils::PrintTemp("%sgameplay_%s.dll", root.GetPath(Root::Path::Binaries), configName.c_str()), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hDLLFile == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		uint64_t lastWriteTime;
		if (GetFileTime(hDLLFile, NULL, NULL, (FILETIME*)&lastWriteTime) && LastWrite != lastWriteTime || root.controls.DebugHotKeyPressed("KEY_O", "KEY_P"))
		{
			pingPong = 1 - pingPong;
			char tmpname[256];
			StringUtils::Printf(tmpname, 256, "%sgameplay_%s.rcpp%i.dll", root.GetPath(Root::Path::Binaries), configName.c_str(), pingPong);

			if (CopyFileA(StringUtils::PrintTemp("%sgameplay_%s.dll", root.GetPath(Root::Path::Binaries), configName.c_str()), tmpname, FALSE))
			{
				LoadGamePlayDll(tmpname);

				LastWrite = lastWriteTime;
			}
		}

		CloseHandle(hDLLFile);

		return true;
	}
	#endif

	void Scripts::SetAllowDynamicReload(bool val)
	{
		allowDynamicReload = val;
	}

	void Scripts::Update()
	{
		if (allowDynamicReload)
		{
			CheckGamePlayDll();
		}
	}

	bool Scripts::Start()
	{
		if (allowDynamicReload)
		{
			CheckGamePlayDll();
		}
		else
		{
			LoadGamePlayDll(StringUtils::PrintTemp("%sgameplay_%s.dll", root.GetPath(Root::Path::Binaries), configName.c_str()));
		}

		return true;
	}

	void Scripts::Stop()
	{
	}
}