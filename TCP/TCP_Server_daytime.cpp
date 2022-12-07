#include <iostream>
#include <chrono>
#include <ctime>
#include <cstdlib> // exit() - стандартные функции Си
#include <cstring> // strcpy(), strlen() - строки Си
#include <unistd.h> // close() - стандартные функции Unix
#include <netinet/in.h> //sockaddr_in and include <sys/socket.h>
#include <arpa/inet.h> // inet_addr()

void errorhandler(const char* why, const int exitCode = 1) {
	std::cerr << why << std::endl;
	exit(exitCode);
}
int main(int argc, char** argv) {

	// подготовить структуру с адресом нашей программы (сервера)
	sockaddr_in* selfAddr = new (sockaddr_in);
	selfAddr->sin_family = AF_INET;
	selfAddr->sin_port = htons(44214);
	selfAddr->sin_addr.s_addr = inet_addr("127.0.0.1");
	//создать сокет
	int mySocket = socket(AF_INET, SOCK_STREAM, 0); // TCP
	if (mySocket == -1) {
		errorhandler("Error open socket", 11);
	}
	// связать сокет с адресом
	int rc = bind(mySocket, (const sockaddr*)selfAddr, sizeof(sockaddr_in));
	if (rc == -1) {
		close(mySocket);
		errorhandler("Error bind socket with local address", 12);
	}
	// поставить сокет в режим ожидания соединения
	rc = listen(mySocket, 3);
	if (rc == -1) {
		close(mySocket);
		errorhandler("Error listen socket with local address", 13);
	}
	while (true) {
		// подготовить буфер для передачи и приема данных
		char* buf = new char[1024];
		int msgLen = strlen(buf);
		// подготовить структуру с адресом "на той стороне" (клиента)
		sockaddr_in* remoteAddr = new (sockaddr_in);
		remoteAddr->sin_family = AF_INET; // интернет протокол IPv4
		remoteAddr->sin_port = 0;
		remoteAddr->sin_addr.s_addr = 0; // локальный адрес 12
		// Принять соединение
		sockaddr_in* client_addr = new sockaddr_in;
		socklen_t len = sizeof(sockaddr_in);
		int worker;
		worker = accept(mySocket, (sockaddr*)(client_addr), &len);
		if (rc == -1) {
			close(worker);
			errorhandler("Error accept socket with local address", 14);
		}
		// принять запрос
		rc = recv(worker, buf, 1024, 0);
		if (rc == -1) {
			close(worker);
			errorhandler("Error receive answer", 15);
		}
		buf[rc] = '\0';
		std::cout << "Server receive: " << buf << std::endl;
		// time
		auto now = std::chrono::system_clock::now();
		std::time_t end_time = std::chrono::system_clock::to_time_t(now);
		strcpy(buf, std::ctime(&end_time));
		// передать данные
		rc = send(worker, buf, 1024, 0);
		if (rc == -1) {
			close(worker);
			errorhandler("Error send message", 16);
		}
		std::cout << "Server send: " << buf << std::endl;
		// закрыть сокет
		close(worker);
		delete remoteAddr;
		delete[] buf;
	}
	// закрыть сокет	
	delete selfAddr;
	return 0;
}