#pragma once

#include "Support.h"
#include "imgui.h"

/**
\ingroup gr_code_common
*/

namespace Orin::ImGuiHelper
{
	CLASS_DECLSPEC ImVec4 Vec4ToImVec4(Math::Vector4 value);
	CLASS_DECLSPEC Math::Vector4 ImVec4ToVec4(ImVec4 value);
	CLASS_DECLSPEC void VerticalHorizontalPadding();
	CLASS_DECLSPEC void HorizontalPadding();
	CLASS_DECLSPEC bool InputString(const char* id, eastl::string& value);
	CLASS_DECLSPEC bool InputCombobox(const char* id, int& index, eastl::vector<eastl::string>& names, eastl::string& namesList);
	CLASS_DECLSPEC void GetAllStyles(eastl::vector<eastl::string>& names);
	CLASS_DECLSPEC void LoadStyle(const char* name);
	CLASS_DECLSPEC void SaveStyle(const char* name);
}
