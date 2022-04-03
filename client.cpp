// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char const *argv[])
{
    int PORT= 7000;
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char *hello = "CHUP";
	char buffer[1024] = {0};
	sock = socket(AF_INET, SOCK_STREAM, 0);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	// Convert IPv4 and IPv6 addresses from text to binary form
	// inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

	connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	send(sock , hello , strlen(hello) , 0 );
	printf("Hello message sent\n");
	return 0;
}
