
#include <SDKDDKVer.h>

#include "Root/Root.h"
#include <iostream>
#include <filesystem>
#include "Windows.h"

#pragma comment(lib, "Rpcrt4.lib")

using namespace Orin;

struct SourceFile
{
	eastl::string path;
	eastl::string filter;
};
	

void ScanSources(const char* basePath, const char* path, eastl::vector<SourceFile>& sources, eastl::vector<eastl::string>& filters)
{
	char croppedPath[MAX_PATH];
	StringUtils::GetCropPath(basePath, path, croppedPath, MAX_PATH);

	if (!std::filesystem::exists(path))
	{
		return;
	}

	for (auto& entry : std::filesystem::directory_iterator(path))
	{
		const char* entryPath = entry.path().u8string().c_str();
		char cropped[MAX_PATH];
		StringUtils::GetCropPath(basePath, entryPath, cropped, MAX_PATH);

		if (entry.is_directory())
		{				
			filters.push_back(cropped);
			ScanSources(basePath, entryPath, sources, filters);
		}
		else
		{
			SourceFile sourceFile;
			sourceFile.path = "../Code/" + eastl::string(cropped);
			sourceFile.filter = croppedPath;

			sources.push_back(sourceFile);
		}		
	}
}

int main(int argc, char* argv[])
{
	if (argc <= 1)
	{
		std::cout << "VcGenProj: Please provide path to a project";

		return 1;
	}

	char exePath[MAX_PATH];
	GetModuleFileNameA(NULL, exePath, MAX_PATH);

	char curDir[MAX_PATH];
	StringUtils::GetPath(exePath, curDir);
	StringUtils::FixSlashes(curDir);

	const char* projectDir = argv[1];

	char vcprojDir[MAX_PATH];
	StringUtils::Printf(vcprojDir, MAX_PATH, "%s/VcProj", projectDir);

	CreateDirectoryA(vcprojDir, nullptr);

	std::cout << vcprojDir << std::endl;

	char codeDir[MAX_PATH];
	StringUtils::Printf(codeDir, MAX_PATH, "%s/Code/", projectDir);
	StringUtils::FixSlashes(codeDir);

	eastl::vector<SourceFile> sources;
	eastl::vector<eastl::string> filters;

	sources.push_back({ StringUtils::PrintTemp("%s/ENgine/CppBuild/src/gameplay.cpp", curDir), "_Orin" });
	sources.push_back({StringUtils::PrintTemp("%s/ENgine/CppBuild/src/gameplay.h", curDir), "_Orin" });

	filters.push_back("_Orin");

	ScanSources(codeDir, codeDir, sources, filters);

	char vcslnPath[MAX_PATH];
	StringUtils::Printf(vcslnPath, MAX_PATH, "%s/gameplay.sln", vcprojDir);
	CopyFileA(StringUtils::PrintTemp("%s/ENgine/CppBuild/vc/gameplay.sln", curDir), vcslnPath , false);

	char vcprojPath[MAX_PATH];
	StringUtils::Printf(vcprojPath, MAX_PATH, "%s/gameplay.vcxproj", vcprojDir);

	char vcprojFiltersPath[MAX_PATH];
	StringUtils::Printf(vcprojFiltersPath, MAX_PATH, "%s/gameplay.vcxproj.filters", vcprojDir);

	char vcprojUserPath[MAX_PATH];
	StringUtils::Printf(vcprojUserPath, MAX_PATH, "%s/gameplay.vcxproj.user", vcprojDir);
	CopyFileA(StringUtils::PrintTemp("%s/ENgine/CppBuild/vc/gameplay.vcxproj.user", curDir), vcprojUserPath, false);

	FileInMemory outputFile;

	if (outputFile.Load(StringUtils::PrintTemp("%s/ENgine/CppBuild/vc/gameplay.vcxproj", curDir)))
	{
		eastl::string data = (const char*)outputFile.GetData();

		StringUtils::FindAndReplace(data, "$EDITOR_DIR", curDir);

		eastl::string sourcesCombined;

		for (auto& entry : sources)
		{
			sourcesCombined += "   <";
			sourcesCombined += (entry.path.find(".cpp") != eastl::string::npos) ? "ClCompile" : "ClInclude";
			sourcesCombined += " Include = \"";
			sourcesCombined += entry.path;
			sourcesCombined += "\" />\n";
		}
		
		StringUtils::FindAndReplace(data, "$SOURCES", sourcesCombined);

		File cmakeOut;
		
		if (cmakeOut.Open(vcprojPath, File::ModeType::WriteText))
		{
			cmakeOut.Write(data.c_str(), (int)data.size());
		}
	}

	if (outputFile.Load(StringUtils::PrintTemp("%s/ENgine/CppBuild/vc/gameplay.vcxproj.filters", curDir)))
	{
		eastl::string data = (const char*)outputFile.GetData();

		eastl::string sourcesCombined;

		for (auto& entry : sources)
		{
			const char* tag = (entry.path.find(".cpp") != eastl::string::npos) ? "ClCompile" : "ClInclude";

			sourcesCombined += "   <";
			sourcesCombined += tag;
			sourcesCombined += " Include = \"";
			sourcesCombined += entry.path;
			sourcesCombined += "\">\n";

			sourcesCombined += "   <Filter>" + entry.filter + "</Filter>\n";

			sourcesCombined += "</" + eastl::string(tag) + ">\n";
		}

		StringUtils::FindAndReplace(data, "$SOURCES", sourcesCombined);

		eastl::string filtersCombined;

		for (auto& entry : filters)
		{
			filtersCombined += "   <Filter Include = \"" + entry + "\">\n";

			UUID uuid;
			RPC_STATUS ret_val = ::UuidCreate(&uuid);

			CHAR* wszUuid = nullptr;
			::UuidToStringA(&uuid, (RPC_CSTR*)&wszUuid);

			filtersCombined += "      <UniqueIdentifier>{" + eastl::string(wszUuid) + "}</UniqueIdentifier>\n";
			filtersCombined += "   </Filter>\n";
		}
		
		StringUtils::FindAndReplace(data, "$FILTERS", filtersCombined);

		File cmakeOut;

		if (cmakeOut.Open(vcprojFiltersPath, File::ModeType::WriteText))
		{
			cmakeOut.Write(data.c_str(), (int)data.size());
		}
	}	

	return 0;
}