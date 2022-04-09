// Client side C/C++ program to demonstrate Socket programming
#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

int main(int argc, char const *argv[])
{
	int PORT = 7002;
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	std::string hello = "Hello, this is the message";
	const char *msg2 = hello.c_str();
	char *buffer = new char[1024];
	sock = socket(AF_INET, SOCK_STREAM, 0);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(serv_addr.sin_zero), '\0', 8); // zero the rest of the struct
	// Convert IPv4 and IPv6 addresses from text to binary form
	// inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

	int server = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	printf("Server connected- %d\n",server);
	std::string s;
	while (true)
	{
		valread = recv(sock, buffer, sizeof(buffer), 0);
		buffer[valread]='\0';
		printf("Valread = %d\n",valread);
		printf("Buffer = %s\n", buffer);
		std::cin >> s;
		msg2 = s.c_str();
		int val = send(sock, msg2, strlen(msg2), 0);
		printf("%d : %s -message sent\n",val, msg2);
	}
	return 0;
}
