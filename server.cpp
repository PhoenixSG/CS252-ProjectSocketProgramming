// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
int main(int argc, char const *argv[])
{
	int PORT = 7000;
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};
	std::string hello2 = "Hello from server";
	const char *hello = hello2.c_str();

	// Creating socket file descriptor
	server_fd = socket(AF_INET, SOCK_STREAM, 0);

	// Forcefully attaching socket to the port 8080
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Forcefully attaching socket to the port 8080
	bind(server_fd, (struct sockaddr *)&address, sizeof(address));
	listen(server_fd, 3);
	new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
	valread = read(new_socket, buffer, 1024);
	printf("%s\n", buffer);
	return 0;
}
