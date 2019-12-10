#pragma once

#include <string>
typedef unsigned __int64 SOCKET;

#define SCENE_BUFFER_SIZE 512
#define INPUT_BUFFER_SIZE 64

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

	int client_index = 0;

	void CreateSocket(std::string ip, int port);
	void Update(void);

	void Send(char* data, int numBytes);

	void SetPosition(int id, float& x, float& z, float delta_time);
private:
	void SetNonBlocking(SOCKET socket);
	void Recv(void);

	SOCKET mServerSocket;
};