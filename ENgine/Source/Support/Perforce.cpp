#include "Perforce.h"
#include "Root/Root.h"

#undef SetPort

namespace Oak
{
	bool Perforce::inited = false;
	bool Perforce::connected = false;
	ClientApi Perforce::client;
	Error Perforce::e;
	ClientUser Perforce::ui;
	StrBuf Perforce::msg;

	void Perforce::Init()
	{
		P4Libraries::Initialize(P4LIBRARIES_INIT_ALL, &e);

		if (e.Test())
		{
			e.Fmt(&msg);
			root.Log("Perforce", "%s", msg.Text());
		}
		else
		{
			inited = true;
		}
	}

	bool Perforce::Connect(const char* url, const char* workspace, const char* user)
	{
		if (!inited)
		{
			root.Log("Perforce", "P4Libraries wasn't initialized");
			return false;
		}

		StrBuf sbPort = url;// "ssl:52.7.105.9:1666";
		StrBuf sbClient = workspace;// "eugene.solyanov_MSI_8764";
		StrBuf sbUser = user;// "eugene.solyanov";

		client.SetPort(&sbPort);
		client.SetClient(&sbClient);
		client.SetUser(&sbUser);

#ifndef _DEBUG
		client.Init(&e);
#endif

		if (e.Test())
		{
			e.Fmt(&msg);
			root.Log("Perforce", "%s", msg.Text());
		}
		else
		{
			connected = true;
		}

		return connected;
	}

	bool Perforce::Checkout(const char* fileName)
	{
		if (!connected)
		{
			return false;
		}

		const char* argv[] = { "-c", "default", fileName };
		int argc = 3;

		root.Log("Perforce", "checkout %s", fileName);

		client.SetArgv(argc, (char* const*)argv);
		client.Run("edit", &ui);

		return true;
	}

	int Perforce::GetRevision()
	{
		if (connected)
		{
			const char* argv[] = { "-m", "1", "//depot/..." };
			int argc = 3;

			client.SetArgv(argc, (char* const*)argv);
			client.Run("changes", &ui);

			char buff[1024];
			ui.OutputText(buff, 1024);

			root.Log("Perforce", "%s", buff);

			return 1;
		}

		return 0;
	}

	void Perforce::Close()
	{
		if (connected)
		{
			client.Final(&e);

			if (e.Test())
			{
				e.Fmt(&msg);
				root.Log("Perforce", "%s", msg.Text());
			}

			connected = false;
		}

		if (inited)
		{
			P4Libraries::Shutdown(P4LIBRARIES_INIT_ALL, &e);

			inited = false;
		}
	}
}