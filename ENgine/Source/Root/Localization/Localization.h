#pragma once
#include "Support/Support.h"

namespace Orin
{
	class CLASS_DECLSPEC Localization
	{
		struct Item
		{
			eastl::map<eastl::string, eastl::string> localized;
		};

		eastl::map<eastl::string, Item> items;
		
		eastl::string curLocale;

	public:

		void LoadLocale();
		void DetectAndSetLocale();
		void SetCurrentLocale(const char* locale);
		const char* GetCurrentLocale();
		const char* GetLocalized(eastl::string& key);
	};
}