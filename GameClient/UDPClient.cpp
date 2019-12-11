#pragma warning(disable:4996) 
#include "UDPClient.h"

#include <bitset>
#include <iostream>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")

#include <string>
#include "Mathf.h"

using std::string;

#define PrintWSAError() _PrintWSAError(__FILE__, __LINE__)

struct sockaddr_in si_other;

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

void UDPClient::SetPosition(int id, float& x, float& z, float& rotation, float delta_time)
{
	x = Mathf::lerp(x, players[id].x, delta_time * 2.f);
	z = Mathf::lerp(z, players[id].z, delta_time * 2.f);
	rotation = players[id].y;
}

UDPClient::UDPClient(void)
	: mServerSocket(INVALID_SOCKET)
{
	players.resize(8);

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
}

UDPClient::~UDPClient(void)
{
	closesocket(mServerSocket);
	WSACleanup();
}

void UDPClient::SetNonBlocking(SOCKET socket)
{
	ULONG NonBlock = 1;
	int result = ioctlsocket(socket, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR)
	{
		PrintWSAError();
		return;
	}
}

void UDPClient::CreateSocket(string ip, int port)
{
	mServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mServerSocket == SOCKET_ERROR)
	{
		PrintWSAError();
		exit(1);
	}

	memset((char*)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(port);
	si_other.sin_addr.s_addr = inet_addr(ip.c_str());
	//InetPton(AF_INET, ip.c_str(), &si_other.sin_addr.s_addr);
	SetNonBlocking(mServerSocket);
}

void UDPClient::Update(void)
{
	Recv();
}

void UDPClient::Recv(void)
{
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	char buffer[SCENE_BUFFER_SIZE];

	int result = recvfrom(mServerSocket, buffer, SCENE_BUFFER_SIZE, 0, (struct sockaddr*) & si_other, &slen);
	if (result == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			return;
		}
		PrintWSAError();

		// For a TCP connection you would close this socket, and remove it from 
		// your list of connections. For UDP we will clear our buffer, and just
		// ignore this.
		memset(buffer, '\0', SCENE_BUFFER_SIZE);
		return;
	}

	// NumPlayers
	// Each player: { x, y }
	unsigned int numPlayers;
	//int index;
	memcpy(&numPlayers, &(buffer[0]), sizeof(unsigned int));
	//memcpy(&index, &(buffer[4]), sizeof(int));

	//client_index = index;

	for (unsigned int i = 0; i < numPlayers; i++)
	{
		float x, y, z;
		memcpy(&x, &(buffer[i * 12 + 4]), sizeof(float));
		memcpy(&y, &(buffer[i * 12 + 8]), sizeof(float));
		memcpy(&z, &(buffer[i * 12 + 12]), sizeof(float));
		players[i].x = x;
		players[i].y = y;
		players[i].z = z;
	}

	//unsigned short port = si_other.sin_port;
	//printf("%d : %hu received %d bytes\n", mServerSocket, port, result);

	printf("%d players: {", numPlayers);
	for (unsigned int i = 0; i < numPlayers; i++)
	{
		printf(" {x: %.2f, y: %.2f}", players[i].x, players[i].z);
	}
	printf(" }\n");
}

void UDPClient::Send(char* data, int numBytes)
{
	int result = sendto(mServerSocket, data, numBytes, 0,
		(struct sockaddr*) & si_other, sizeof(si_other));

	if (result == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK) return;
		PrintWSAError();
		return;
	}

	if (result == 0)
	{
		printf("Disconnected...\n");
		return;
	}

	// printf("Number of bytes sent: %d\n", result);
}