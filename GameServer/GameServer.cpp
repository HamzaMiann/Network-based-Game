#include "GameServer.h"

#include <winsock.h>
#include <WS2tcpip.h>
#include <ctime>

struct Player {
	unsigned short port;
	struct sockaddr_in si_other;
	float x;
	float y;
	bool up, down, right, left;
};
struct Bullet
{
	float velX;
	float velY;
	float x;
	float y;
};

const float PLAYER_DIAMETER = 1.0f;
const float BULLET_DIAMETER = 1.0f;

float calcDistance(float x1, float y1, float x2, float y2)
{
	return sqrt(((x2-x1)*(x2-x1)) + ((y2-y1)*(y2-y1)));
}

unsigned int numPlayersConnected = 0;

std::vector<Player> mPlayers;
std::vector<Bullet> mBullets;

const float UPDATES_PER_SEC = 5;
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
	fprintf(stderr, "[WSAError:%d] %s\n", WSAErrorCode, s);
	LocalFree(s);
}

UDPServer::UDPServer(void)
	:m_IsRunning(false),
	m_ListenSocket(INVALID_SOCKET),
	m_AcceptSocket(INVALID_SOCKET)
{
	mPlayers.resize(8);

	//WinSock vars
	WSAData WSAData;
	int iResult;
	int Port = 5150;
	SOCKADDR_IN ReceiverAddr;

	//Initialize winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (iResult != 0) {
		PrintWSAError();
		return;
	}

	//Create a socket
	m_ListenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_ListenSocket == INVALID_SOCKET) {
		PrintWSAError();
		return;
	}

	//Bind socket
	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_port = htons(Port);
	ReceiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	iResult = bind(m_ListenSocket, (SOCKADDR*)&ReceiverAddr, sizeof(ReceiverAddr));
	if (iResult == SOCKET_ERROR) {
		PrintWSAError();
		return;
	}

	//Set socket to nonblocking
	SetNonBlocking(m_ListenSocket);

	//Server is ready
	printf("[SERVER] Receiving IP: %s\n", inet_ntoa(ReceiverAddr.sin_addr));
	printf("[SERVER] Receiving Port: %d\n", htons(ReceiverAddr.sin_port));
	printf("[SERVER] Ready to receive a datagram...\n");

	m_IsRunning = true;
	prev = std::clock();
}

UDPServer::~UDPServer(void)
{
	closesocket(m_ListenSocket);
	WSACleanup();
}

void UDPServer::SetNonBlocking(SOCKET socket)
{
	ULONG NonBlock = 1;
	int result = ioctlsocket(socket, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR) {
		PrintWSAError();
		return;
	}
}

void UDPServer::Update(void)
{
	if (!m_IsRunning) return;

	ReadData();

	curr = std::clock();
	elapsed_secs = (curr - prev) / double(CLOCKS_PER_SEC);
	
	if (elapsed_secs < (1.0f / UPDATES_PER_SEC)) return;
	prev = curr;

	UpdatePlayers();

	//Get if a player has been hit
	for (Bullet b : mBullets)
	{
		b.x += b.velX;
		b.y += b.velY;
		for (Player p : mPlayers)
		{
			if (calcDistance(p.x, p.y, b.x, b.y) < BULLET_DIAMETER + PLAYER_DIAMETER)
			{
				//TODO: Player was shot
			}
		}
	}
	BroadcastUpdate();
}

void UDPServer::UpdatePlayers(void)
{
	for (int i = 0; i < numPlayersConnected; i++)
	{
		//TODO: Update each player
	}
}

void UDPServer::BroadcastUpdate(void)
{
	const int DEFAULT_BUFLEN = 512;
	char buffer[512];
	memset(buffer, '\0', DEFAULT_BUFLEN);

	memcpy(&(buffer[0]), &numPlayersConnected, sizeof(unsigned int));

	for (int i = 0; i < numPlayersConnected; i++)
	{
		//TODO: add each new info
	}
	int result = sendto(m_ListenSocket, buffer, 12, 0,
		(struct sockaddr*) & (mPlayers[0].si_other), sizeof(mPlayers[0].si_other));
}

Player* GetPlayerByPort(unsigned short port, struct sockaddr_in si_other)
{
	//If a player with the port has already connect, return it
	for (int i = 0; i < mPlayers.size(); i++)
	{
		if (mPlayers[i].port == port) return &(mPlayers[i]);
	}

	//Otherwise create a new player
	mPlayers[numPlayersConnected].port = port;
	//TODO: create default player variables

	mPlayers[numPlayersConnected].si_other = si_other;
	return &(mPlayers[numPlayersConnected++]);
}

void UDPServer::ReadData(void)
{
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	char buffer[512];

	int result = recvfrom(m_ListenSocket, buffer, 512, 0, (struct sockaddr*) & si_other, &slen);
	if (result == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			return;
		}
		PrintWSAError();

		memset(buffer, '\0', 512);
		return;
	}

	unsigned short port = si_other.sin_port;

	Player* player = GetPlayerByPort(port, si_other);

	//TODO: copy what commands player used into the buffer


	//TODO: Send data back to client

}