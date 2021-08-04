
#include "Root/Root.h"

#include "Root/Scenes/SceneEntity.h"
#include "Editor/Editor.h"

#ifdef OAK_EDITOR
#include <filesystem>
#endif

namespace Oak
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

	void Scripts::CompileProjectCode(bool forceCompile)
	{
		if (forceCompile || !std::filesystem::exists(StringUtils::PrintTemp("%s/gameplay_%s.dll", root.GetRootPath(), configName.c_str())))
		{
			CreateDirectoryA(StringUtils::PrintTemp("%s/_Code", root.GetRootPath()), nullptr);

			char curDir[256];
			GetCurrentDirectoryA(256, curDir);

			char path[256];
			StringUtils::Printf(path, 256, "%s/_Code/CMakeLists.txt", root.GetRootPath());

			CopyFileA(StringUtils::PrintTemp("%s/ENgine/CppBuild/CMakeLists.txt", curDir), path, false);

			SetCurrentDirectoryA(StringUtils::PrintTemp("%s/_Code", root.GetRootPath()));

			if (system("cmake CMakeLists.txt") != 0)
			{
				MESSAGE_BOX("Can't compile script", "CMake is missing. Please install CMake");
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

							if (StringUtils::IsEqual(lineVersion.c_str(), "2019"))
							{
								reader.LeaveBlock();

								reader.Read("productPath", vsPath);

								if (system(StringUtils::PrintTemp("%s gameplay.sln /Build %s", vsPath.c_str(), configName.c_str())) == 0)
								{
									char tmpname[256];
									StringUtils::Printf(tmpname, 256, "%s/gameplay_%s.dll", root.GetRootPath(), configName.c_str());

									CopyFileA(StringUtils::PrintTemp("%s/_Code/%s/gameplay.dll", root.GetRootPath(), configName.c_str()), tmpname, FALSE);
								}
							}
							else
							{
								MESSAGE_BOX("Can't compile script", "Visual Studio was found but it is not 2019. Please install Visual Studio 2019");
							}
						}
					}
					else
					{
						MESSAGE_BOX("Can't compile script", "Visual Studio was found but can't be launched. Please reinstall Visual Studio 2019");
					}

					free(programFiles);
				}
				else
				{
					MESSAGE_BOX("Can't compile script", "Visual Studio was not found. Please install Visual Studio 2019");
				}
			}

			SetCurrentDirectoryA(curDir);
		}

		CheckGamePlayDll();
	}

	void Scripts::LoadGamePlayDll(const char* path)
	{
		HMODULE newModule = LoadLibraryA(path);

		uint32_t uid = 0;

		if (newModule)
		{
			if (Module && allowDynamicReload)
			{
				if (editor.selectedEntity)
				{
					uid = editor.selectedEntity->GetUID();
					editor.SelectEntity(nullptr);
				}

				void* Procs = GetProcAddress((HMODULE)Module, "unregister_code");
				auto code_ptr = (decltype(unregister_code)*)Procs;

				code_ptr(ClassFactorySceneEntity::Decls());
			}

			{
				void* Procs = GetProcAddress((HMODULE)newModule, "register_code");
				auto code_ptr = (decltype(register_code)*)Procs;

				code_ptr(ClassFactorySceneEntity::Decls());

				ClassFactorySceneEntity::Sort();
			}

			if (Module && allowDynamicReload)
			{
				{
					void* Procs = GetProcAddress(newModule, "recreate_entites");
					auto code_ptr = (decltype(recreate_entites)*)Procs;

					code_ptr((eastl::vector<SceneEntity*>&)Oak::editor.project.selectedScene->scene->GetEntities());
				}

				FreeLibrary((HMODULE)Module);

				char tmpname[256];
				StringUtils::Printf(tmpname, 256, "%s/_Code/%s/Gameplay.rcpp%i.dll", root.GetRootPath(), configName.c_str(), 1 - pingPong);
				DeleteFileA(tmpname);
			}

			Module = newModule;

			if (uid != 0)
			{
				editor.SelectEntity(Oak::editor.project.selectedScene->scene->FindEntity(uid));
			}
		}
	}

	void Scripts::CheckGamePlayDll()
	{
		HANDLE hDLLFile = CreateFileA(StringUtils::PrintTemp("%s/gameplay_%s.dll", root.GetRootPath(), configName.c_str()), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hDLLFile == INVALID_HANDLE_VALUE)
		{
			return;
		}

		uint64_t lastWriteTime;
		if (GetFileTime(hDLLFile, NULL, NULL, (FILETIME*)&lastWriteTime) && LastWrite != lastWriteTime)
		{
			pingPong = 1 - pingPong;
			char tmpname[256];
			StringUtils::Printf(tmpname, 256, "%s/_Code/%s/gameplay.rcpp%i.dll", root.GetRootPath(), configName.c_str(), pingPong);

			if (CopyFileA(StringUtils::PrintTemp("%s/gameplay_%s.dll", root.GetRootPath(), configName.c_str()), tmpname, FALSE))
			{
				LoadGamePlayDll(tmpname);

				LastWrite = lastWriteTime;
			}
		}

		CloseHandle(hDLLFile);
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
			LoadGamePlayDll(StringUtils::PrintTemp("%s/gameplay_%s.dll", root.GetRootPath(), configName.c_str()));
		}

		return true;
	}

	void Scripts::Stop()
	{
	}
}