#include "Support/Support.h"

namespace Orin
{
	class CLASS_DECLSPEC Perforce
	{
		static bool connectionChecked;
		static bool isConnected;
		static int  currentCL;
		static eastl::string config;
		static eastl::vector<eastl::string> cmdResult;

	public:

		static void SetConfig(const char* url, const char* workspace, const char* user);
		static void SetCurrentCL(int cl);
		static bool CheckConnection();

		static bool AddToDepot(const char* fileName);
		static bool Checkout(const char* fileName);
		static int  GetRevision();
	};
}