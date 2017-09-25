#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define BUF_SIZE 1024	  // array size
#define TIMEOUT  20000    // 20 seconds

#include <winsock2.h>
#include "common.h"

class MySocket {
public:
	void winsock_test(string url); 
	bool Receive(string &recv_string); 

	// add your functions, e.g., SendRequest(...), ParseURL(...), ParseFile(...)

private:
	char buf[BUF_SIZE];  // used for recv() 
	SOCKET sock; 
};