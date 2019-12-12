#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <vector>
#include <ctime>
#include "Transform.h"

#pragma comment(lib, "Ws2_32.lib")

using std::vector;

void _PrintWSAError(const char* file, int line);
#define PrintWSAError() _PrintWSAError(__FILE__, __LINE__)

#define SCENE_BUFFER_SIZE 1024
#define INPUT_BUFFER_SIZE 16

static int global_id = 0;

const float UPDATES_PER_SEC = 5;

struct ServerPlayer
{
	// player id for client
	unsigned int id = global_id++;
	char is_alive = 1;


	// server id by port
	unsigned short port;
	struct sockaddr_in si_other;
	unsigned int request_id = 0;

	Transform transform;
	bool up, down, right, left;

	char can_shoot = true;
	float reload = 2.f;
};

struct Projectile
{
	// is alive?
	char state = 1;

	float duration = 5.f;

	// position
	glm::vec2 pos;
	
	// velocity
	glm::vec2 vel;
};

class GameServer
{
public:
	GameServer(void);
	~GameServer(void);

	void Update(void);

private:
	unsigned int state_id = 0;
	unsigned int numPlayersConnected = 0;
	std::vector<ServerPlayer> mPlayers;
	std::vector<Projectile> mProjectiles;

	std::clock_t curr;
	std::clock_t prev;
	double elapsed_secs;

	void SetNonBlocking(SOCKET socket);
	void ReadData(void);

	void UpdatePlayers(void);
	void BroadcastUpdate(void);

	ServerPlayer* GetPlayerByPort(unsigned short port, struct sockaddr_in si_other);

	bool mIsRunning;

	FD_SET mReadSet;
	timeval mTv;
	SOCKET mAcceptSocket;
	SOCKET mListenSocket;
};
