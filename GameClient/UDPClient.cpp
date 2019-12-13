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
#include <string>
#include "Mathf.h"

#pragma comment(lib, "Ws2_32.lib")

using std::string;
struct sockaddr_in si_other;

#define PrintWSAError() _PrintWSAError(__FILE__, __LINE__)
#define SET(to, from, type) memcpy(&(to), &(from), sizeof(type)); index += sizeof(type)
//#define SET(from, type) memcpy(&(buffer[index]), &from, sizeof(type)); index += sizeof(type)
#define INIT_INDEX(start) int index = start

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

		memset(buffer, '\0', SCENE_BUFFER_SIZE);
		return;
	}

	//int index = 0;
	INIT_INDEX(0);

	unsigned int state = 0;
	SET(client_id, buffer[index], unsigned int);

	// Client-side reconciliation
	SET(state, buffer[index], unsigned int);
	if (state < state_id) return;
	state_id = state;

	// GET PLAYER DATA
	memcpy(&numPlayers, &(buffer[index]), sizeof(unsigned int)); index += sizeof(unsigned int);

	for (unsigned int i = 0; i < numPlayers; i++)
	{
		float x, y, z;
		char is_alive;
		SET(is_alive, buffer[index], char);
		SET(x, buffer[index], float);
		SET(y, buffer[index], float);
		SET(z, buffer[index], float);
		players[i].x = x;
		players[i].y = y;
		players[i].z = z;
		players[i].is_alive = is_alive;
	}

	// GET PROJECTILE DATA

	unsigned int numProjectiles;
	SET(numProjectiles, buffer[index], unsigned int);

	if (numProjectiles > projectiles.size())
	{
		projectiles.resize(numProjectiles);
	}
	for (int i = 0; i < numProjectiles; ++i)
	{
		char is_active;
		float x, z, vx, vz;
		SET(is_active, buffer[index], char);
		SET(x, buffer[index], float);
		SET(z, buffer[index], float);
		SET(vx, buffer[index], float);
		SET(vz, buffer[index], float);
		projectiles[i].pos = glm::vec3(x, 50.f, z);
		projectiles[i].state = is_active;
		projectiles[i].vel = glm::vec3(vx, 0.f, vz);
	}

}

void UDPClient::Send(char* data, int numBytes)
{
	char buffer[INPUT_BUFFER_SIZE];
	memset(buffer, '\0', INPUT_BUFFER_SIZE);

	memcpy(&(buffer[0]), &send_id, sizeof(unsigned int));
	++send_id;

	for (int i = 0; i < numBytes; ++i)
	{
		buffer[4 + i] = data[i];
	}

	int result = sendto(mServerSocket, buffer, INPUT_BUFFER_SIZE, 0,
		(struct sockaddr*) & si_other, sizeof(si_other));

	if (result == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK) return;
		PrintWSAError();
		exit(1);
	}

	if (result == 0)
	{
		printf("Disconnected...\n");
		exit(1);
	}

	// printf("Number of bytes sent: %d\n", result);
}