#include <iostream>
#include <vector>
#include <algorithm>

#include "ServerSocket.h"

int main() {
	ServerSocket::InitWinsock();
	ServerSocket serverSocket(NULL, "23456");
	//ConnectionThread test(nullptr);
	//fdsfdsasd
	//vector<tuple<ConnectionThread, std::thread>> clientThreads;
	//
	//clientThreads.push_back({test, test.StartConnection()});
	//
	//std::for_each(clientThreads.begin(), clientThreads.end(), [](tuple<ConnectionThread, std::thread> &t) {
	//	get<1>(t).join();
	//});
}