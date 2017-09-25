#pragma once

#include "crawl.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <map>

class CrawlerParameters {
public:	
	int count;
	HANDLE mutex;
	HANDLE eventQuit;
	std::queue<string> Q;
	std::map<std::string, boolean> seenIPS;
	std::map<std::string, boolean> seenHosts;
	unsigned int active_threads;
	unsigned int numCrawls;
	double total_links;
	std::map<std::string, boolean> Hash;

	int E = 0;
	int H = 0;
	int D = 0;
	int R = 0;
	int C = 0;
	int L = 0;

};

UINT CrawlerFunction(LPVOID pParam) {
	CrawlerParameters *p = ((CrawlerParameters*)pParam);
	//MySocket s;
	string url = "";
	ofstream fout;
	WSADATA wsaData;
	//Initialize WinSock; once per program run
	WORD wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		printf("WSAStartup error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	while (true)
	{
		WaitForSingleObject(p->mutex, INFINITE);
		if (p->Q.size() == 0) {
			ReleaseMutex(p->mutex);
			break;
		}
		else {
			url = p->Q.front();
			p->Q.pop();
			p->E++;
			url = url.substr(7, url.length() - 1);

			int backslash = url.find("/");
			if (backslash != -1)
				url = url.substr(0, backslash);

			if (!p->Hash[url]) {
				p->H++;
				p->Hash.insert(std::make_pair(url, true));
				MySocket s;
				s.winsock_test(url, pParam);
				fout << url << endl;
			}
		}
		
	}
	WSACleanup();
	return 0;
}
int main(int argc, const char* argv[])
{
	int numThreads;
	string filename = "";

	if (argc > 1)
	{
		numThreads = atoi(argv[1]); // convert c string to integer
		filename = argv[2];
		printf("number of threads is %d, filename is %s\n\n", numThreads, filename.c_str());
	}
	
	filename = "URL-input-100.txt";
	ifstream fin;
	ofstream fout;
	fin.open(filename);
	fout.open("crawldata.txt");

	string url = "";

	if (fin.fail())
	{ printf("NO such a file. Failed to open it\n"); return 0; }

	CrawlerParameters cp;
	

	while (!fin.eof()) {
		fin >> url;
		cout << url << endl;
		cp.Q.push(url);
	}

	cp.mutex = CreateMutex(NULL, 0, NULL);
	cp.eventQuit = CreateEvent(NULL, false, false, NULL);

	HANDLE aThread[10];

	for (int i = 0; i < 10; i++) {
		aThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CrawlerFunction, &cp, 0, NULL);
	}

	fin.close(); fout.close();

	// call cleanup when done with everything and ready to exit program
	//WSACleanup();

	printf("\nHit Enter to continue ... \n");
	getchar();  // the black output screen will remain there until you hit any char

	return 0;
}

string findURL(string url) {
	string newURL = "";
	int backslash = url.find("/"); 
	
	if (backslash != -1)
		newURL = url.substr(0, backslash);
	
	return url;
}

//int findPort(string url) {
//	int port;
//	int colon = url.find(":");
//
//	if (colon != -1) {
//		string temp = url.substr(colon + 1, url.length() - 1);
//		int backslash = temp.find("/");
//		temp = url.substr(colon + 1, backslash);
//		return stoi(temp);
//	}
//	return 80;
//}