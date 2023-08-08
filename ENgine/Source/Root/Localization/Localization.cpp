#include "Root/Root.h"

namespace Orin
{
	void Localization::LoadLocale()
	{
		JsonReader reader;

		if (reader.ParseFile("locale"))
		{
			while (reader.EnterBlock("item"))
			{
				eastl::string key;
				reader.Read("key", key);

				Item item;

				while (reader.EnterBlock("localized"))
				{
					eastl::string locale;
					reader.Read("locale", locale);

					eastl::string text;
					reader.Read("text", text);

					item.localized[locale] = text;

					reader.LeaveBlock();
				}
						
				items[key] = item;

				reader.LeaveBlock();
			}
		}
	}

	void Localization::DetectAndSetLocale()
	{
		GetUserDefaultUILanguage();
	}

	void Localization::SetCurrentLocale(const char* locale)
	{
		curLocale = locale;
	}

	const char* Localization::GetLocalized(eastl::string& key)
	{
		auto iter = items.find(key);

		if (iter != items.end())
		{
			auto& localized = iter->second.localized;
			auto iterLoc = iter->second.localized.find(curLocale);

			if (iterLoc != localized.end())
			{
				return iterLoc->second.c_str();
			}
			
			iterLoc = iter->second.localized.find("en");

			if (iterLoc != localized.end())
			{
				return iterLoc->second.c_str();
			}
		}

		return "Missing";
	}
}