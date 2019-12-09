#include "GameServer.h"

#include <winsock.h>
#include <WS2tcpip.h>

#include <ctime>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

struct Player
{
	unsigned short port; // their id;
	struct sockaddr_in si_other;
	float x;
	float y;
	bool up, down, right, left;
};

unsigned int numPlayersConnected = 0;

std::vector<Player> mPlayers;

const float UPDATES_PER_SEC = 5;		// 5Hz / 200ms per update / 5 updates per second
std::clock_t curr;
std::clock_t prev;
double elapsed_secs;


void _PrintWSAError(const char* file, int line)
{
	int WSAErrorCode = WSAGetLastError();
	wchar_t* s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				   NULL, WSAErrorCode,
				   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				   (LPWSTR)&s, 0, NULL);
	fprintf(stderr, "[WSAError:%d] %S\n", WSAErrorCode, s);
	LocalFree(s);
}

UDPServer::UDPServer(void)
	: mIsRunning(false)
	, mListenSocket(INVALID_SOCKET)
	, mAcceptSocket(INVALID_SOCKET)
{
	mPlayers.resize(8);

	// WinSock vars
	WSAData		WSAData;
	int			iResult;
	int			Port = 5150;
	SOCKADDR_IN ReceiverAddr;

	// Step #0 Initialize WinSock
	iResult = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (iResult != 0)
	{
		PrintWSAError();
		return;
	}

	// Step #1 Create a socket
	mListenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mListenSocket == INVALID_SOCKET)
	{
		PrintWSAError();
		return;
	}

	// Step #2 Bind our socket
	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_port = htons(Port);
	ReceiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	iResult = bind(mListenSocket, (SOCKADDR*)&ReceiverAddr, sizeof(ReceiverAddr));
	if (iResult == SOCKET_ERROR)
	{
		PrintWSAError();
		return;
	}

	// Set our socket to be nonblocking
	SetNonBlocking(mListenSocket);

	// Our server is ready 
	char ipbuf[INET_ADDRSTRLEN];
	printf("[SERVER] Receiving IP: %s\n", inet_ntop(AF_INET, &ReceiverAddr.sin_addr, ipbuf, sizeof(ipbuf)));
	printf("[SERVER] Receiving Port: %d\n", htons(ReceiverAddr.sin_port));
	printf("[SERVER] Ready to receive a datagram...\n");

	mIsRunning = true;
	prev = std::clock();
} // end UDPServer

UDPServer::~UDPServer(void)
{
	closesocket(mListenSocket);
	WSACleanup();	// <-- Not necessary if quitting application, Windows will handle this.
}

void UDPServer::SetNonBlocking(SOCKET socket)
{
	ULONG NonBlock = 1;
	int result = ioctlsocket(socket, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR)
	{
		PrintWSAError();
		return;
	}
}

void UDPServer::Update(void)
{
	if (!mIsRunning) return;

	// TODO: ReadData, SendData
	ReadData();

	curr = std::clock();
	elapsed_secs = (curr - prev) / double(CLOCKS_PER_SEC);

	if (elapsed_secs < (1.0f / UPDATES_PER_SEC)) return;
	prev = curr;

	UpdatePlayers();
	BroadcastUpdate();
}

void UDPServer::UpdatePlayers(void)
{
	for (int i = 0; i < numPlayersConnected; i++)
	{
		if (mPlayers[i].up) mPlayers[i].y += 10.0f * elapsed_secs;
		if (mPlayers[i].down) mPlayers[i].y -= 10.0f * elapsed_secs;
		if (mPlayers[i].right) mPlayers[i].x += 10.0f * elapsed_secs;
		if (mPlayers[i].left) mPlayers[i].x -= 10.0f * elapsed_secs;
	}
}

void UDPServer::BroadcastUpdate(void)
{
	// create our data to send, then send the same data to all players
	const int DEFAULT_BUFLEN = 512;
	char buffer[512];
	memset(buffer, '\0', DEFAULT_BUFLEN);

	memcpy(&(buffer[0]), &numPlayersConnected, sizeof(unsigned int));

	for (int i = 0; i < numPlayersConnected; i++)
	{
		float x = mPlayers[i].x;
		float y = mPlayers[i].y;
		memcpy(&(buffer[i * 8 + 4]), &x, sizeof(float));
		memcpy(&(buffer[i * 8 + 8]), &y, sizeof(float));
	}

	int result = sendto(mListenSocket, buffer, 12, 0,
		(struct sockaddr*) & (mPlayers[0].si_other), sizeof(mPlayers[0].si_other));
}



Player* GetPlayerByPort(unsigned short port, struct sockaddr_in si_other)
{
	// If a player with this port is already connected, return it
	for (int i = 0; i < mPlayers.size(); i++)
	{
		if (mPlayers[i].port == port) return &(mPlayers[i]);
	}

	// Otherwise create a new player, and return that one!
	mPlayers[numPlayersConnected].port = port;
	mPlayers[numPlayersConnected].x = 0.0f;
	mPlayers[numPlayersConnected].y = 0.0f;
	mPlayers[numPlayersConnected].si_other = si_other;
	return &(mPlayers[numPlayersConnected++]);
}

void UDPServer::ReadData(void)
{
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	char buffer[512];

	int result = recvfrom(mListenSocket, buffer, 512, 0, (struct sockaddr*) & si_other, &slen);
	if (result == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			// printf(".");		// Quick test
			return;
		}
		PrintWSAError();

		// For a TCP connection you would close this socket, and remove it from 
		// your list of connections. For UDP we will clear our buffer, and just
		// ignore this.
		memset(buffer, '\0', 512);
		return;
	}


	unsigned short port = si_other.sin_port;

	Player* player = GetPlayerByPort(port, si_other);

	player->up = buffer[0] == 1;
	player->down = buffer[1] == 1;
	player->right = buffer[2] == 1;
	player->left = buffer[3] == 1;

	printf("%d : %hu received { %d %d %d %d }\n", mListenSocket,
		   port, player->up, player->down, player->right, player->left);

	// Send the data back to the client
	// result = sendto(mListenSocket, buffer, 1, 0, (struct sockaddr*) & si_other, sizeof(si_other));
}

