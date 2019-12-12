#include "GameServer.h"

#include <winsock.h>
#include <WS2tcpip.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define SET(to, from, type) memcpy(&(to), &from, sizeof(type)); index += sizeof(type)
#define SET(from, type) memcpy(&(buffer[index]), &from, sizeof(type)); index += sizeof(type)
#define INIT_INDEX int index = 0
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

		if (player->is_alive != 1) continue;

		float speed = 0.f;
		if (player->up) speed = 5.f;
		if (player->down) speed += -5.f;

		if (player->right) player->transform.pos.y -= 90.f * (float)elapsed_secs;
		if (player->left) player->transform.pos.y += 90.f * (float)elapsed_secs;

		player->transform.pos += player->transform.Forward() * speed * (float) elapsed_secs;

		if (!player->can_shoot == 1)
		{
			player->reload -= elapsed_secs;
			player->can_shoot = (player->reload < 0.f)? 1 : 0;
		}

		//if (player->up) player->transform.pos.z += 5.0f * (float)elapsed_secs;
		//if (player->down) player->transform.pos.z -= 5.0f * (float)elapsed_secs;
		//if (player->right) player->transform.pos.x -= 5.0f * (float)elapsed_secs;
		//if (player->left) player->transform.pos.x += 5.0f * (float)elapsed_secs;
		//printf(" %d : { %f, %f } { %f } || ", player->id, player->transform.pos.x, player->transform.pos.z, player->transform.pos.y);
	}
	//printf("\n");

	for (unsigned int i = 0; i < mProjectiles.size(); ++i)
	{
		Projectile* p = &mProjectiles[i];
		if (p->state == 1)
		{
			p->duration -= elapsed_secs;
			if (p->duration < 0)
			{
				p->state = 0;
			}
			p->pos += p->vel * (float)elapsed_secs;

			for (unsigned int n = 0; n < numPlayersConnected; ++n)
			{
				ServerPlayer* player = &mPlayers[n];
				if (glm::distance2(p->pos, glm::vec2(player->transform.pos.x, player->transform.pos.z)) < 1.1f)
				{
					player->is_alive = 0;
					p->state = 0;
					break;
				}
			}
		}
	}

}



void GameServer::BroadcastUpdate(void)
{
	// create our data to send, then send the same data to all players
	const int DEFAULT_BUFLEN = SCENE_BUFFER_SIZE;
	char buffer[SCENE_BUFFER_SIZE];
	memset(buffer, '\0', DEFAULT_BUFLEN);

	//int index = 4;
	INIT_INDEX(4);

	// space for client_id
	//memcpy(&(buffer[index]), '\0', sizeof(unsigned int)); index += sizeof(unsigned int);

	SET(state_id, unsigned int);
	//memcpy(&(buffer[index]), &state_id, sizeof(unsigned int)); index += sizeof(unsigned int);
	state_id++;

	memcpy(&(buffer[index]), &numPlayersConnected, sizeof(unsigned int)); index += sizeof(unsigned int);


	for (unsigned int i = 0; i < numPlayersConnected; i++)
	{
		SET(mPlayers[i].is_alive, char);
		SET(mPlayers[i].transform.pos.x, float);
		SET(mPlayers[i].transform.pos.y, float);
		SET(mPlayers[i].transform.pos.z, float);
		//memcpy(&(buffer[index]), &mPlayers[i].is_alive, sizeof(char)); index += sizeof(char);
		//memcpy(&(buffer[index]), &mPlayers[i].transform.pos.x, sizeof(float)); index += sizeof(float);
		//memcpy(&(buffer[index]), &mPlayers[i].transform.pos.y, sizeof(float)); index += sizeof(float);
		//memcpy(&(buffer[index]), &mPlayers[i].transform.pos.z, sizeof(float)); index += sizeof(float);
	}

	unsigned int size = mProjectiles.size();
	SET(size, unsigned int);
	//memcpy(&(buffer[index]), &size, sizeof(unsigned int)); index += sizeof(unsigned int);

	for (unsigned int i = 0; i < size; i++)
	{
		SET(mProjectiles[i].state, char);
		SET(mProjectiles[i].pos.x, float);
		SET(mProjectiles[i].pos.y, float);
		//memcpy(&(buffer[index]), &mProjectiles[i].state, sizeof(char)); index += sizeof(char);
		//memcpy(&(buffer[index]), &mProjectiles[i].pos.x, sizeof(float)); index += sizeof(float);
		//memcpy(&(buffer[index]), &mProjectiles[i].pos.y, sizeof(float)); index += sizeof(float);
	}

	for (unsigned int i = 0; i < numPlayersConnected; i++)
	{
		//SET(buffer[0], i, unsigned int);
		memcpy(&(buffer[0]), &mPlayers[i].id, sizeof(unsigned int));
		int result = sendto(mListenSocket, buffer, SCENE_BUFFER_SIZE, 0,
			(struct sockaddr*) & (mPlayers[i].si_other), sizeof(mPlayers[i].si_other));
	}
}



ServerPlayer* GameServer::GetPlayerByPort(unsigned short port, struct sockaddr_in si_other)
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

	// Server reconciliation
	unsigned int request = 0;
	memcpy(&request, &(buffer[0]), sizeof(unsigned int));
	if (request < player->request_id) return;

	player->request_id = request;

	player->up = buffer[4] == 1;
	player->down = buffer[5] == 1;
	player->right = buffer[6] == 1;
	player->left = buffer[7] == 1;
	if (buffer[8] == 1 && player->can_shoot == 1 && player->is_alive == 1)
	{
		mProjectiles.resize(mProjectiles.size() + 1);
		glm::vec3 pos = player->transform.Forward();
		mProjectiles[mProjectiles.size() - 1].vel = glm::vec2(pos.x, pos.z) * 5.f;
		pos *= 1.3f;
		pos.x += player->transform.pos.x;
		pos.z += player->transform.pos.z;
		mProjectiles[mProjectiles.size() - 1].pos = glm::vec2(pos.x, pos.z);

		player->can_shoot = 0;
		player->reload = 2.f;
		printf("shot ball :O\n");
	}
	if (buffer[9] == 1 && player->is_alive == 0)
	{
		player->is_alive = 1;
		player->can_shoot = 1;
		player->transform = Transform();
	}

	//printf("%d : %hu received { %d %d %d %d }\n", player->id, port, player->up, player->down, player->right, player->left);
	//printf("%d : %hu received { %d %d %d %d }\n", (int)mListenSocket, port, player->up, player->down, player->right, player->left);

	// Send the data back to the client
	// result = sendto(mListenSocket, buffer, 1, 0, (struct sockaddr*) & si_other, sizeof(si_other));
}

