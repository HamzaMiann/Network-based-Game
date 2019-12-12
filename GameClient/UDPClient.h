#pragma once

#include <string>
#include <vector>
#include <glm/vec3.hpp>
typedef unsigned __int64 SOCKET;

#define SCENE_BUFFER_SIZE 1024
#define INPUT_BUFFER_SIZE 16

class cGameObject;

struct Player
{
	char is_alive;
	float x, y, z;
};

struct Projectile
{
	// is alive?
	char state = 1;
	// position
	glm::vec3 pos;
	// previous position
	glm::vec3 previousPos = glm::vec3(0.f);
};

class UDPClient
{
	UDPClient(void);
public:
	~UDPClient(void);

	static UDPClient* Instance()
	{
		static UDPClient instance;
		return &instance;
	}

	std::vector<Player> players;
	std::vector<Projectile> projectiles;
	std::vector<cGameObject*>* playerObjects;

	int client_id = 0;
	unsigned int numPlayers = 0;

	unsigned int send_id = 0;
	unsigned int state_id = 0;


	void CreateSocket(std::string ip, int port);
	void Update(void);

	void Send(char* data, int numBytes);

	void SetPosition(int id, float& x, float& z, float & rotation, float delta_time);

private:
	void SetNonBlocking(SOCKET socket);
	void Recv(void);

	SOCKET mServerSocket;
};