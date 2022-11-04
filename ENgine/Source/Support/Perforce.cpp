#include "Perforce.h"
#include "Utils.h"
#include "Root/Root.h"

#undef SetPort

namespace Oak
{
	bool Perforce::connectionChecked = true;
	bool Perforce::isConnected = false;
	eastl::string Perforce::config;
	eastl::vector<eastl::string> Perforce::cmdResult;

	void Perforce::SetConfig(const char* url, const char* workspace, const char* user)
	{
		config = StringUtils::PrintTemp("p4.exe -p %s -c %s -u %s ", url, workspace, user);
		connectionChecked = false;
	}

	bool Perforce::CheckConnection()
	{
		if (connectionChecked)
		{
			return isConnected;
		}

		connectionChecked = true;

		eastl::string cmd = config + "info";

		if (Utils::Execute(cmd.c_str(), cmdResult))
		{			
			isConnected = cmdResult.size() > 1;
		}

		return isConnected;
	}

	bool Perforce::Checkout(const char* fileName)
	{
		if (!CheckConnection())
		{
			return false;
		}

		eastl::string cmd = config + StringUtils::PrintTemp("edit -c default %s", fileName);

		root.Log("Perforce", "checkout %s", fileName);

		if (Utils::Execute(cmd.c_str(), cmdResult))
		{
			return (cmdResult.size() > 0 && strstr(cmdResult[0].c_str(), "opened for edit"));
		}

		return false;
	}

	bool Perforce::AddToDepot(const char* fileName)
	{
		if(!CheckConnection())
		{
			return false;
		}

		eastl::string cmd = config + StringUtils::PrintTemp("add -c default %s", fileName);

		root.Log("Perforce", "checkout %s", fileName);

		if (Utils::Execute(cmd.c_str(), cmdResult))
		{
			return (cmdResult.size() > 0 && strstr(cmdResult[0].c_str(), "opened for add"));
		}

		return false;
	}

	int Perforce::GetRevision()
	{
		if (!CheckConnection())
		{
			return 0;
		}

		eastl::string cmd = config + "changes -m1 //depot/...";

		if (Utils::Execute(cmd.c_str(), cmdResult))
		{
			if (cmdResult.size() == 1)
			{
				const char* str = cmdResult[0].c_str();

				if (strstr(str, "Change "))
				{
					return atoi(&str[7]);
				}
			}
		}

		return 0;
	}
}