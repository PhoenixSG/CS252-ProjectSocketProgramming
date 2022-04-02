#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <fstream>
#include <sstream>

void server(int port)
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in address;
	int addrlen = sizeof(address);

    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(int));
    bind(server_socket, (struct sockaddr *)&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    	char buffer[1024] = {};

    if (listen(server_socket, 10) != -1)
    {
        new_client = accept(server_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_client > -1)
        {
            client_msg_len = read(new_client, buffer, 1024);
            if (client_msg_len > 0)
            {
                printf("%s\n", buffer);
                flag_0[buffer[0] - '0'] = true;
            }
        }
    }
}