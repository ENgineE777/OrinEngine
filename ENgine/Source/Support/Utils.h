#pragma once

#include "Support/Support.h"

/**
\ingroup gr_code_common
*/

namespace Orin::Utils
{
	CLASS_DECLSPEC bool Execute(const char* command, eastl::vector<eastl::string>& result);
}
