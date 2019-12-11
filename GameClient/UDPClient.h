#pragma once

#include <string>
#include <vector>
typedef unsigned __int64 SOCKET;

#define SCENE_BUFFER_SIZE 1024
#define INPUT_BUFFER_SIZE 16

class cGameObject;

struct Player
{
	float x, y, z;
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
	std::vector<cGameObject*>* playerObjects;

	int client_index = 0;

	void CreateSocket(std::string ip, int port);
	void Update(void);

	void Send(char* data, int numBytes);

	void SetPosition(int id, float& x, float& z, float & rotation, float delta_time);

private:
	void SetNonBlocking(SOCKET socket);
	void Recv(void);

	SOCKET mServerSocket;
};