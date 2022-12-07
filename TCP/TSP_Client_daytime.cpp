#include <iostream>
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
	// подготовить структуру с адресом нашей программы (клиента)
	sockaddr_in* selfAddr = new (sockaddr_in);
	selfAddr->sin_family = AF_INET;
	selfAddr->sin_port = 0;
	selfAddr->sin_addr.s_addr = 0;
	// подготовить структуру с адресом "на той стороне" (сервера)
	sockaddr_in* remoteAddr = new (sockaddr_in);
	remoteAddr->sin_family = AF_INET; // интернет протокол IPv4
	remoteAddr->sin_port = htons(44214);
	remoteAddr->sin_addr.s_addr = inet_addr("127.0.0.1"); // локальный адрес 12
	// подготовить буфер для передачи и приема данных
	char* buf = new char[256];
	strcpy(buf, "hi its tcp client\n"); //копируем строку
	int msgLen = strlen(buf);
	//вычисляем длину строки
	// // создать сокет
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
	// установить соединение
	rc = connect(mySocket, (const sockaddr*)remoteAddr, sizeof(sockaddr_in));
	if (rc == -1) {
		close(mySocket);
		errorhandler("Error connect socket with remote server", 13);
	}
	// передать данные
	rc = send(mySocket, buf, msgLen, 0);
	if (rc == -1) {
		close(mySocket);
		errorhandler("Error send message", 14);
	}
	std::cout << "Client send: " << buf << std::endl;
	// принять ответ
	rc = recv(mySocket, buf, 256, 0);
	if (rc == -1) {
		close(mySocket);
		errorhandler("Error receive answer", 15);
	}
	buf[rc] = '\0';
	std::cout << "Client receive: " << buf << std::endl;
	// закрыть сокет
	close(mySocket);
	delete selfAddr;
	delete remoteAddr;
	delete[] buf;
	return 0;
}