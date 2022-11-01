#include "Support/Support.h"

namespace Oak
{
	class CLASS_DECLSPEC Perforce
	{
		static bool connectionChecked;
		static bool isConnected;
		static eastl::string config;
		static eastl::vector<eastl::string> cmdResult;

	public:

		static void SetConfig(const char* url, const char* workspace, const char* user);
		static bool CheckConnection();

		static bool AddToDepot(const char* fileName);
		static bool Checkout(const char* fileName);
		static int  GetRevision();
	};
}