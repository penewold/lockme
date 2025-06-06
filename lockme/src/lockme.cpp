/* --- [ WHAT IS THIS ] ---
 * Lockme is a simple program to lock this computer remotely from a phone on the same network.
 * A counterpart android app should be used to lock the computer.
 *
 * --- [ STRUCTURE ] ---
 * - define variables like port and lockword
 * - init udp/datagram socket
 * - listen for incoming requests. If the request matches the request lockword, lock the computer.
 *
 * --- [ USAGE ] ---
 * computer: add program to start on boot to run in background. Possibly configure a file with the lockword and port.
 * phone: install app. Configure lockword and port in a settings page. Press the big button on the main page to broadcast the lock request.
*/

#include <iostream>
#include <cstdlib>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

#ifdef _DEBUG 
int main()
#else 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	int port = 7492; // Port to listen on  
	const char* lockword = "laasmig"; // Lockword to match for locking the computer  
	char lmCommand[256];
	char pingCommand[256];

	sprintf_s(lmCommand, "lm.%s", lockword);
	sprintf_s(pingCommand, "ping.%s", lockword);

	char lmCommandRecv[256];
	char pingCommandRecv[256];

	sprintf_s(lmCommandRecv, "lmrecv.%s", lockword);
	sprintf_s(pingCommandRecv, "pingrecv.%s", lockword);


	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
		return 1;
	}

	SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udpSocket == INVALID_SOCKET) {
		std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	sockaddr_in serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces  
	serverAddr.sin_port = htons(port);

	if (bind(udpSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
		closesocket(udpSocket);
		WSACleanup();
		return 1;
	}

	std::cout << "UDP server is listening" << std::endl;

	char buffer[1024];
	sockaddr_in clientAddr;
	int clientAddrSize = sizeof(clientAddr);

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));


		int bytesReceived = recvfrom(
			udpSocket,
			buffer,
			sizeof(buffer) - 1,
			0,
			(sockaddr*)&clientAddr,
			&clientAddrSize
		);

		if (bytesReceived == SOCKET_ERROR) {
			std::cerr << "recvfrom failed: " << WSAGetLastError() << std::endl;
			break;
		}

		buffer[bytesReceived] = '\0'; // Null-terminate the received data
		char clientIP[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));

		std::cout << "Received from " << clientIP << ":" << ntohs(clientAddr.sin_port)
			<< " - " << buffer << std::endl;

		

		// test for lock command
		if (strcmp(buffer, lmCommand) == 0) {
			sendto(
				udpSocket,
				lmCommandRecv,
				strlen(lmCommandRecv),
				0,
				(sockaddr*)&clientAddr,
				clientAddrSize
			);

			#ifndef _DEBUG 
			system("rundll32.exe user32.dll,LockWorkStation"); // Lock the workstation
			#endif
			continue;
		}

		if (strcmp(buffer, pingCommand) == 0) {
			sendto(
				udpSocket,
				pingCommandRecv,
				strlen(pingCommandRecv),
				0,
				(sockaddr*)&clientAddr,
				clientAddrSize
			);
			continue;
		}
	}

	// Cleanup  
	closesocket(udpSocket);
	WSACleanup();
	return 0;
}

