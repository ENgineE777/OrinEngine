#include "ImGuiHelper.h"
#include "Root/Files/Files.h"
#include <filesystem>
#include "Root/Root.h"

namespace Oak::ImGuiHelper
{
	ImVec4 Vec4ToImVec4(Math::Vector4 value)
	{
		return ImVec4(value.x, value.y, value.z, value.w);
	}

	Math::Vector4 ImVec4ToVec4(ImVec4 value)
	{
		return Math::Vector4(value.x, value.y, value.z, value.w);
	}

	void VerticalHorizontalPadding()
	{
		ImGui::Dummy(ImVec2(0.0f, 3.0f));
		ImGui::Dummy(ImVec2(3.0f, 3.0f));
		ImGui::SameLine();
	}

	void HorizontalPadding()
	{
		ImGui::Dummy(ImVec2(3.0f, 3.0f));
		ImGui::SameLine();
	}

	bool InputString(const char* id, eastl::string& value)
	{
		struct Funcs
		{
			static int ResizeCallback(ImGuiInputTextCallbackData* data)
			{
				if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
				{
					eastl::string* str = (eastl::string*)data->UserData;
					str->resize(data->BufSize + 1);
					data->Buf = str->begin();
				}
				return 0;
			}
		};

		return ImGui::InputText(id, value.begin(), (size_t)value.size() + 1, ImGuiInputTextFlags_CallbackResize, Funcs::ResizeCallback, (void*)&value);
	}

	bool InputCombobox(const char* id, int& index, eastl::vector<eastl::string>& names, eastl::string& namesList)
	{
		if (namesList.empty())
		{
			int count = 0;

			for (int i = 0; i < names.size(); i++)
			{
				count += (int)names[i].size() + 1;
			}

			namesList.resize(count + 1);

			int index = 0;

			for (int i = 0; i < names.size(); i++)
			{
				int sz = (int)names[i].size() + 1;
				memcpy(&namesList[index], names[i].c_str(), sz);
				index += sz;
			}
		}

		return ImGui::Combo(id, &index, namesList.c_str());
	}

	void GetAllStyles(eastl::vector<eastl::string>& names)
	{
		names.clear();

		for (auto& entry : std::filesystem::directory_iterator("ENgine/editor/themes/"))
		{
			if (!entry.is_directory())
			{
				names.push_back(entry.path().filename().string().c_str());
			}
		}
	}

	void LoadStyle(const char* name)
	{
		ImGuiStyle& style = ImGui::GetStyle();

		JsonReader reader;

		root.Log("sdfds", std::filesystem::current_path().string().c_str());

		if (reader.ParseFile(StringUtils::PrintTemp("%s/ENgine/editor/themes/%s", std::filesystem::current_path().string().c_str(), name)))
		{
			if (reader.EnterBlock("colors"))
			{
				for (int i = 0; i < ImGuiCol_COUNT; i++)
				{
					auto color = ImVec4ToVec4(style.Colors[i]);
					reader.Read(ImGui::GetStyleColorName(i), color);
					style.Colors[i] = Vec4ToImVec4(color);
				}

				reader.LeaveBlock();
			}
		}
	}

	void SaveStyle(const char* name)
	{
		ImGuiStyle& style = ImGui::GetStyle();

		JsonWriter writer;

		if (writer.Start(StringUtils::PrintTemp("%s/ENgine/editor/themes/%s", std::filesystem::current_path().string().c_str(), name)))
		{
			writer.StartBlock("colors");

			for (int i = 0; i < ImGuiCol_COUNT; i++)
			{
				auto color = ImVec4ToVec4(style.Colors[i]);
				writer.Write(ImGui::GetStyleColorName(i), color);
			}

			writer.FinishBlock();
		}
	}
}
