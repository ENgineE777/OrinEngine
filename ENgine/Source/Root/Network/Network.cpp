#include "Network.h"
#include "Root/Root.h"


#ifdef PLATFORM_WINDOWS
#pragma comment (lib, "Ws2_32.lib")
#endif

namespace Orin
{
	NetworkClient::NetworkClient()
	{
		send_buffer = (char*)malloc(BUFFSIZE);
		recv_buffer = (char*)malloc(BUFFSIZE);
	}

	NetworkClient::~NetworkClient()
	{
		free(send_buffer);
		free(recv_buffer);
	}

	bool NetworkClient::Connect(const char* ip, int port)
	{
	#ifdef PLATFORM_WINDOWS
		struct addrinfo *result = nullptr, *ptr = nullptr;
		struct addrinfo hints;

		WSADATA wsaData;

		int res = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (res != 0)
		{
			root.Log("Network","WSAStartup failed with error: %d", res);
			return false;
		}

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		char strPort[128];
		StringUtils::Printf(strPort, 128, "%i", port);

		res = getaddrinfo(ip, strPort, &hints, &result);

		if (res != 0)
		{
			root.Log("Network", "getaddrinfo failed with error: %d", res);
			return false;
		}

		for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
		{
			tcpSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

			if (tcpSocket == INVALID_SOCKET)
			{
				root.Log("Network", "socket failed with error: %ld", WSAGetLastError());
				return false;
			}

			res = connect(tcpSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

			if (res == SOCKET_ERROR)
			{
				closesocket(tcpSocket);
				tcpSocket = INVALID_SOCKET;
				root.Log("Network", "The server is down... did not connect");
			}
		}

		freeaddrinfo(result);

		if (tcpSocket == INVALID_SOCKET)
		{
			root.Log("Network", "Unable to connect to server!");
			return false;
		}
		u_long iMode = 1;

		res = ioctlsocket(tcpSocket, FIONBIO, &iMode);
	
		if (res == SOCKET_ERROR)
		{
			root.Log("Network", "ioctlsocket failed with error: %d", WSAGetLastError());
			closesocket(tcpSocket);
			return false;
		}

		char value = 1;
		setsockopt(tcpSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));
	#endif

		return true;
	}

	#ifdef PLATFORM_WINDOWS
	void NetworkClient::SetSocket(int id, SOCKET socket)
	{
		this->tcpSocket = socket;
		this->id = id;
	}
	#endif

	void NetworkClient::Send(void* data, int len)
	{
		CheckCapacity(send_buffer, send_buff_size, send_len + len);

		memcpy(&send_buffer[send_len], data, len);
		send_len += len;
	}

	void NetworkClient::Recive()
	{
	#ifdef PLATFORM_WINDOWS
		char chunk[CHUNKSIZE];

		char* buffer = recv_buffer;

		while (true)
		{
			int rec = recv(tcpSocket, chunk, CHUNKSIZE, 0);

			if (rec == -1)
			{
				break;
			}

			if (rec == 0)
			{
				root.Log("Network", "Connection closed");
				closesocket(tcpSocket);
				break;
			}

			recv_len += rec;

			CheckCapacity(recv_buffer, recv_buff_size, recv_len);

			memcpy(buffer, chunk, rec);
			buffer += rec;

			if (rec < CHUNKSIZE)
			{
				break;
			}
		}
	#endif
	}

	void NetworkClient::Update()
	{
		Recive();

		if (recv_len)
		{
			if (delegedate)
			{
				delegedate->OnDataRecieved(id, recv_buffer, recv_len);
			}

			recv_len = 0;
		}

	#ifdef PLATFORM_WINDOWS
		if (send_len)
		{
			send(tcpSocket, send_buffer, send_len, 0);
			send_len = 0;
		}
	#endif
	}

	bool NetworkServer::Start(int port)
	{
	#ifdef PLATFORM_WINDOWS
		WSADATA wsaData;

		int res = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (res != 0)
		{
			root.Log("Network", "WSAStartup failed with error: %d", res);
			return false;
		}

		struct addrinfo *result = nullptr;
		struct addrinfo hints;

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		char strPort[32];
		StringUtils::Printf(strPort, 32, "%i", port);

		res = getaddrinfo(NULL, strPort, &hints, &result);

		if (res != 0)
		{
			root.Log("Network", "getaddrinfo failed with error: %d", res);
			return false;
		}

		listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

		if (listenSocket == INVALID_SOCKET)
		{
			root.Log("Network", "socket failed with error: %ld", WSAGetLastError());
			freeaddrinfo(result);
			return false;
		}

		u_long iMode = 1;
		res = ioctlsocket(listenSocket, FIONBIO, &iMode);

		if (res == SOCKET_ERROR)
		{
			root.Log("Network", "ioctlsocket failed with error: %d", WSAGetLastError());
			closesocket(listenSocket);
			return false;
		}

		res = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);

		if (res == SOCKET_ERROR)
		{
			int k = WSAGetLastError();
			root.Log("Network", "bind failed with error: %d", WSAGetLastError());
			freeaddrinfo(result);
			closesocket(listenSocket);
			return false;
		}

		freeaddrinfo(result);

		res = listen(listenSocket, SOMAXCONN);

		if (res == SOCKET_ERROR)
		{
			root.Log("Network", "listen failed with error: %d", WSAGetLastError());
			closesocket(listenSocket);
			return false;
		}
	#endif

		return true;
	}

	void NetworkServer::Update()
	{
	#ifdef PLATFORM_WINDOWS
		SOCKET clientSocket = accept(listenSocket, NULL, NULL);

		if (clientSocket != INVALID_SOCKET)
		{
			char value = 1;
			setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

			client_id++;

			NetworkClient* client = new NetworkClient();
			client->SetSocket(client_id, clientSocket);

			client->delegedate = delegedate;

			clients.push_back(client);

			if (delegedate)
			{
				delegedate->OnClientConnected(client_id);
			}
		}
	#endif

		for (auto client : clients)
		{
			client->Update();
		}
	}

	void NetworkServer::Send2Client(int id, void* data, int len)
	{
		for (auto client : clients)
		{
			if (client->id == id)
			{
				client->Send(data, len);
				break;
			}
		}
	}

	void NetworkServer::Send2All(void* data, int len)
	{
		for (auto client : clients)
		{
			client->Send(data, len);
		}
	}
}