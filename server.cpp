// Server side C/C++ program to demonstrate Socket programming
#include <bits/stdc++.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
int main(int argc, char const *argv[])
{
	int PORT = 7002;
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {};

	// Creating socket file descriptor
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	std::string hello = "Hello, this is the message";
	const char *msg2 = hello.c_str();
	// Forcefully attaching socket to the port 8080
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	memset(&(address.sin_zero), '\0', 8); // zero the rest of the struct

	// Forcefully attaching socket to the port 8080
	bind(server_fd, (struct sockaddr *)&address, sizeof(address));
	listen(server_fd, 10);
	new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
	printf("%d\n",new_socket);
	while (true)
	{
		std::string s;
		std::cin >> s;
		msg2 = s.c_str();
		int val = send(new_socket, msg2, strlen(msg2), 0);
		printf("%d : %s -message sent\n",val, msg2);
		valread = recv(new_socket, buffer, sizeof(buffer), 0);
		buffer[valread]='\0';
		printf("%d\n",valread);
		printf("%s\n", buffer);
	}

	return 0;
}
