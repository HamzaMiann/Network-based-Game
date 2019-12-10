#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

using std::vector;

void _PrintWSAError(const char* file, int line);
#define PrintWSAError() _PrintWSAError(__FILE__, __LINE__)

template <class T>
static T randInRange(T min, T max)
{
	double value =
		min + static_cast <double> (rand())
		/ (static_cast <double> (RAND_MAX / (static_cast<double>(max - min))));
	return static_cast<T>(value);
};

class UDPServer
{
public:
	UDPServer(void);
	~UDPServer(void);

	void Update(void);

private:
	void SetNonBlocking(SOCKET socket);
	void ReadData(void);

	void UpdatePlayers(void);
	void BroadcastUpdate(void);

	bool mIsRunning;

	FD_SET mReadSet;
	timeval mTv;
	SOCKET mAcceptSocket;
	SOCKET mListenSocket;
};
