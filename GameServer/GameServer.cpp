#include "GameServer.h"

#include <winsock.h>
#include <WS2tcpip.h>

#include <ctime>
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

int global_id = 0;

const glm::vec3 forward(0.f, 0.f, 1.f);

struct Transform
{
	// x is x position
	// y is y ROTATION
	// z is z position
	glm::vec3 pos = glm::vec3(
		randInRange(-20.f, 20.f),
		0.f,
		randInRange(-10.f, 10.f)
	);

	glm::quat Rotation()
	{
		return glm::quat(glm::vec3(0.f, glm::radians(pos.y), 0.f));
	}

	glm::vec3 Forward()
	{
		return Rotation() * forward;
	}

};

struct ServerPlayer
{
	unsigned int id = global_id++;
	unsigned short port; // their id;
	struct sockaddr_in si_other;
	Transform transform;
	bool up, down, right, left;
};

unsigned int numPlayersConnected = 0;
std::vector<ServerPlayer> mPlayers;

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

GameServer::GameServer(void)
	: mIsRunning(false)
	, mListenSocket(INVALID_SOCKET)
	, mAcceptSocket(INVALID_SOCKET)
{
	mPlayers.resize(4);

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

GameServer::~GameServer(void)
{
	closesocket(mListenSocket);
	WSACleanup();	// <-- Not necessary if quitting application, Windows will handle this.
}

void GameServer::SetNonBlocking(SOCKET socket)
{
	ULONG NonBlock = 1;
	int result = ioctlsocket(socket, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR)
	{
		PrintWSAError();
		return;
	}
}

void GameServer::Update(void)
{
	if (!mIsRunning) return;

	ReadData();

	curr = std::clock();
	elapsed_secs = (curr - prev) / double(CLOCKS_PER_SEC);

	if (elapsed_secs < (1.0f / UPDATES_PER_SEC)) return;
	prev = curr;

	UpdatePlayers();
	BroadcastUpdate();
}

void GameServer::UpdatePlayers(void)
{
	for (unsigned int i = 0; i < numPlayersConnected; i++)
	{
		ServerPlayer* player = &mPlayers[i];
		float speed = 0.f;
		if (player->up) speed = 5.f;
		if (player->down) speed += -5.f;

		if (player->right) player->transform.pos.y -= 90.f * (float)elapsed_secs;
		if (player->left) player->transform.pos.y += 90.f * (float)elapsed_secs;

		player->transform.pos += player->transform.Forward() * speed * (float) elapsed_secs;

		//if (player->up) player->transform.pos.z += 5.0f * (float)elapsed_secs;
		//if (player->down) player->transform.pos.z -= 5.0f * (float)elapsed_secs;
		//if (player->right) player->transform.pos.x -= 5.0f * (float)elapsed_secs;
		//if (player->left) player->transform.pos.x += 5.0f * (float)elapsed_secs;
		printf(" %d : { %f, %f } { %f } || ", player->id, player->transform.pos.x, player->transform.pos.z, player->transform.pos.y);
	}
	printf("\n");
}

void GameServer::BroadcastUpdate(void)
{
	// create our data to send, then send the same data to all players
	const int DEFAULT_BUFLEN = SCENE_BUFFER_SIZE;
	char buffer[SCENE_BUFFER_SIZE];
	memset(buffer, '\0', DEFAULT_BUFLEN);

	memcpy(&(buffer[0]), &numPlayersConnected, sizeof(unsigned int));


	for (unsigned int i = 0; i < numPlayersConnected; i++)
	{
		//float x = mPlayers[i].transform.pos.x;
		//float z = mPlayers[i].transform.pos.z;
		memcpy(&(buffer[i * 12 + 4]), &mPlayers[i].transform.pos.x, sizeof(float));
		memcpy(&(buffer[i * 12 + 8]), &mPlayers[i].transform.pos.y, sizeof(float));
		memcpy(&(buffer[i * 12 + 12]), &mPlayers[i].transform.pos.z, sizeof(float));

	}

	for (unsigned int i = 0; i < numPlayersConnected; i++)
	{
		//memcpy(&(buffer[index_offset]), &i, sizeof(int));
		int result = sendto(mListenSocket, buffer, SCENE_BUFFER_SIZE, 0,
			(struct sockaddr*) & (mPlayers[i].si_other), sizeof(mPlayers[i].si_other));
	}
}



ServerPlayer* GetPlayerByPort(unsigned short port, struct sockaddr_in si_other)
{
	// If a player with this port is already connected, return it
	for (int i = 0; i < mPlayers.size(); i++)
	{
		if (mPlayers[i].port == port)
		{
			//printf("Player %d input\n", i);
			return &(mPlayers[i]);
		}
	}

	// Otherwise create a new player, and return that one!
	//printf("New player!\n");
	mPlayers[numPlayersConnected].port = port;
	//mPlayers[numPlayersConnected].x = randInRange(-20, 20);
	//mPlayers[numPlayersConnected].z = randInRange(-20, 20);
	mPlayers[numPlayersConnected].si_other = si_other;
	return &(mPlayers[numPlayersConnected++]);
}

void GameServer::ReadData(void)
{
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	char buffer[INPUT_BUFFER_SIZE];

	int result = recvfrom(mListenSocket, buffer, INPUT_BUFFER_SIZE, 0, (struct sockaddr*) & si_other, &slen);
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
		memset(buffer, '\0', INPUT_BUFFER_SIZE);
		return;
	}


	unsigned short port = si_other.sin_port;

	ServerPlayer* player = GetPlayerByPort(port, si_other);

	player->up = buffer[0] == 1;
	player->down = buffer[1] == 1;
	player->right = buffer[2] == 1;
	player->left = buffer[3] == 1;

	//printf("%d : %hu received { %d %d %d %d }\n", player->id, port, player->up, player->down, player->right, player->left);
	//printf("%d : %hu received { %d %d %d %d }\n", (int)mListenSocket, port, player->up, player->down, player->right, player->left);

	// Send the data back to the client
	// result = sendto(mListenSocket, buffer, 1, 0, (struct sockaddr*) & si_other, sizeof(si_other));
}

