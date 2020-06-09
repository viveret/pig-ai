#include "../Actions.hpp"
#include "../../Program.hpp"
#include "../../AIModelAdapter.hpp"
#include "../../Data/DAOs.hpp"

#include <iostream>

#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

// https://gasnet.lbl.gov/amudp/dist/socket.h
#define INVALID_SOCKET          ((SOCKET)(~0))
#define SOCKET_ERROR            (-1)
#define SD_SEND					(3)
#endif

#include "nlohmann/json.hpp"
#include <tiny_dnn/io/display.h>
#include <limits.h> 

using namespace PigAI;

nlohmann::json readJson(SOCKET client) {
	tiny_dnn::timer t;

	size_t iResult = 0;
	unsigned long bytesRemaining = 0;

	const int recvbuflen = sizeof(std::uint8_t) * 256 * 4;
	char recvbuf[recvbuflen];
	std::vector<std::uint8_t> inputJson;

	// Get content length
	iResult = recv(client, recvbuf, recvbuflen, 0);
	if (iResult >= sizeof(unsigned long long)) {
		bytesRemaining = *((unsigned long long*)recvbuf);
		std::cout << "Content length: " << bytesRemaining << std::endl;

		bytesRemaining -= iResult - sizeof(unsigned long long);

		for (size_t i = sizeof(unsigned long long); i < iResult / sizeof(std::uint8_t); i++) {
			inputJson.push_back(((std::uint8_t*)recvbuf)[i]);
		}
	}
	else {
		std::cout << "missing content length" << std::endl;
		return nullptr;
	}

	if (bytesRemaining > 0 && bytesRemaining < INT_MAX) {
		tiny_dnn::progress_display disp(bytesRemaining);
		do {
			iResult = recv(client, recvbuf, recvbuflen, 0);
			if (iResult > 0) {
				bytesRemaining -= iResult;
				disp += iResult;

				for (size_t i = 0; i < iResult / sizeof(std::uint8_t); i++) {
					inputJson.push_back(((std::uint8_t*)recvbuf)[i]);
				}
			}
			else if (iResult == 0)
				std::cout << "Connection closing..." << std::endl;
			else {
				#ifdef _WIN32
				std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
				closesocket(client);
				WSACleanup();
				#else
				close(client);
				#endif
				return nullptr;
			}
		} while (iResult > 0 && bytesRemaining > 0 && bytesRemaining < INT_MAX);
	}

	std::cout << "Request read finished. " << t.elapsed() << "s elapsed. Parsing..." << std::endl;
	t.restart();

	auto ret = nlohmann::json::from_bson(inputJson);

	std::cout << "Request parse finished. " << t.elapsed() << "s elapsed." << std::endl;
	return ret;
}

DaemonAction::DaemonAction(AIProgram *prog): MenuAction(prog) {

}

const char* DaemonAction::label() {
	return "daemon";
}

std::string DaemonAction::description() {
	std::stringstream ss;
	ss << "Allow local JSON TCP connections to port " << this->_prog->DEFAULT_PORT() << " to classify data";
	return ss.str();
}

void DaemonAction::run() {
	std::string port;
	std::cout << "port: ";
	std::cin >> port;

	struct addrinfo* result = NULL, hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	auto r = getaddrinfo(NULL, port.c_str(), &hints, &result);
	if (r != 0) {
		std::cout << "getaddrinfo failed: " << r;
		#ifdef _WIN32
		WSACleanup();
		#endif
		return;
	}

	// Create a SOCKET for connecting to server
	auto ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		#ifdef _WIN32
		std::cout << "socket failed with error: " << WSAGetLastError();
		WSACleanup();
		#endif
		freeaddrinfo(result);
		return;
	}

	r = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (r == SOCKET_ERROR) {
		#ifdef _WIN32
		std::cout << "bind failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		#else
		close(ListenSocket);
		#endif
		freeaddrinfo(result);
		return;
	}

	freeaddrinfo(result);

	r = listen(ListenSocket, SOMAXCONN);
	if (r == SOCKET_ERROR) {
		#ifdef _WIN32
		std::cout << "listen failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		#else
		close(ListenSocket);
		#endif
		return;
	}

	while (true) {
		// Accept a client socket
		auto ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			#ifdef _WIN32
			std::cout << "accept failed with error: " << WSAGetLastError() << std::endl;
			closesocket(ListenSocket);
			WSACleanup();
			#else
			close(ListenSocket);
			#endif
			return;
		}
		this->incomingRequestNN(ClientSocket);
	}

	#ifdef _WIN32
	closesocket(ListenSocket);
	#else
	close(ListenSocket);
	#endif
}

void DaemonAction::incomingRequestNN(SOCKET client) {
	{
		auto addr = ((struct sockaddr_in*) & client)->sin_addr;
		char addrStr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &addr, addrStr, INET_ADDRSTRLEN);
		std::cout << "Client " << addrStr << " connected." << std::endl;
	}

	tiny_dnn::vec_t input(this->_prog->input_width() * this->_prog->input_width() * this->_prog->input_channels(), 0.0f);
	auto requestJson = readJson(client);
	if (requestJson == nullptr) {
		return;
	}

	nlohmann::json returnJson;

	if (requestJson.is_object()) {
		auto speaker = requestJson["speaker"];
		auto initial_text = requestJson["seed"];

		this->_prog->m_model->test(speaker, initial_text);
		// if (ret_dao) {
		// 	auto ret = ret_dao->asVec();

		// 	int actualOutput = -1;
		// 	double maxConfidence = 0.0f;
		// 	for (size_t categoryIndex = 0; categoryIndex < ret.size(); categoryIndex++) {
		// 		auto confidence = ret.at(categoryIndex);
		// 		if (confidence > maxConfidence) {
		// 			actualOutput = categoryIndex;
		// 			maxConfidence = confidence;
		// 		}
		// 	}

		// 	delete ret_dao;
		// 	returnJson["test"] = actualOutput;
		// } else {
		// 	returnJson["error"] = "Invalid output";
		// }
	}
	else {
		returnJson["error"] = "Invalid request (must be an object)";
	}

	std::vector<std::uint8_t> outputJson = nlohmann::json::to_bson(returnJson);

	// Echo the buffer back to the sender
	auto iSendResult = send(client, (const char*)outputJson.data(), outputJson.size(), 0);
	if (iSendResult == SOCKET_ERROR) {
		#ifdef _WIN32
		std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
		closesocket(client);
		WSACleanup();
		#else
		close(client);
		#endif
		return;
	}
	std::cout << "Response sent, " << iSendResult << " bytes" << std::endl;

	// shutdown the connection since we're done
	auto iResult = shutdown(client, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		#ifdef _WIN32
		std::cout << "shutdown failed with error: " << WSAGetLastError() << std::endl;
		closesocket(client);
		WSACleanup();
		#else
		close(client);
		#endif
		return;
	}

	// cleanup
	#ifdef _WIN32
	closesocket(client);
	#else
	close(client);
	#endif
}
