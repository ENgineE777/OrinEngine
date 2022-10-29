#include "p4/clientapi.h"
#include "p4/p4libs.h"

namespace Oak
{
	class Perforce
	{
		static bool inited;
		static bool connected;
		static ClientApi client;
		static Error e;
		static ClientUser ui;
		static StrBuf msg;

	public:

		static void Init();
		static bool Connect(const char* url, const char* workspace, const char* user);
		static bool Checkout(const char* fileName);
		static int  GetRevision();
		static void Close();
	};
}