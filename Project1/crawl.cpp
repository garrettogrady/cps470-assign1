#pragma once

#include "crawl.h"

void MySocket::winsock_test(string url)
{
	// design an algorithm to extract host, path, query, and port# from the input url

	string host = url; // example url: www.google.com 
	string path = "", query = "";
	int port = 80;

	// for debugging purpose, print host, path, port, query on screen to see if they are correct
	cout << "URL: " << url << endl;
	cout << "host: " << host << endl;
	cout << "port: " << port << endl;
	cout << "path: " << path << endl;
	cout << "query: " << query << endl;

	// open a TCP socket
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		printf("socket() generated error %d\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	// structure used in DNS lookups
	struct hostent *remote;

	// structure for connecting to server
	struct sockaddr_in server;

	// first assume that the string is an IP address
	DWORD IP = inet_addr(host.c_str());
	if (IP == INADDR_NONE)
	{
		// if not a valid IP, then do a lookup
		if ((remote = gethostbyname(host.c_str())) == NULL)
		{
			printf("Invalid string: neither FQDN, nor IP address\n");
			return;
		}
		else // take the first IP address and copy into sin_addr
			memcpy((char *)&(server.sin_addr), remote->h_addr, remote->h_length);
	}
	else
	{
		// if a valid IP, directly drop its binary version into sin_addr
		server.sin_addr.S_un.S_addr = IP;
	}

	// setup the port # and protocol type
	server.sin_family = AF_INET;
	server.sin_port = htons(port);		// host-to-network flips the byte order

	// connect to the server on port 
	if (connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		printf("Connection error: %d\n", WSAGetLastError());
		return;
	}

	printf("Successfully connected to %s (%s) on port %d\n", host.c_str(), inet_ntoa(server.sin_addr), htons(server.sin_port));

	// send GET request to host
	const int reqlen = 80;

	char *ask = "GET / HTTP/1.0\nHost: www.google.com\n\n";
	send(sock, ask, reqlen, 0);

	string recv_string = "";
	// call Receive(recv_string), which invokes recv() in a loop
	Receive(recv_string);

	cout << "\n recive file: \n" << recv_string << endl;
	// print reply on the screen: 

}

// This function receives reply from remote server.
// input: recv_string is a string
// output: after this function is called, recv_string stores reply from remote server
bool MySocket::Receive(string & recv_string)
{
	FD_SET Reader; // for select() function call
	FD_ZERO(&Reader);
	FD_SET(sock, &Reader); // add your socket to the set Reader
	// set timeout, used for select()
	struct timeval timeout;
	timeout.tv_sec = TIMEOUT; // must include <time.h>
	timeout.tv_usec = 0;
	recv_string = ""; // empty string, used to save all received data
	int bytes = 0; // count how many bytes received via each recv()
	do{
		if (select(0, &Reader, NULL, NULL, &timeout) > 0) // if have data
		{
			if ((bytes = recv(sock, buf, BUF_SIZE - 1, 0)) == SOCKET_ERROR)
			{
				printf("failed with %d on recv\n", WSAGetLastError());
				return false;
			}
			else if (bytes > 0)
			{
				buf[bytes] = 0; // NULL terminate buffer
				recv_string += buf; // append to the string recv_string
			}
			// quit loop if it hits the maximum size, i.e., 2 MB for pages, 16 KB for robots 
			else
			{
				// timed out on select()
				return false;
			}
		}
	} while (bytes > 0); // end of do-while 
		return true;

}
