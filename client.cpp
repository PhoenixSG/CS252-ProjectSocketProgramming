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
void client(int port)
{
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in neighbour_address;

    neighbour_address.sin_family = AF_INET;
    neighbour_address.sin_port = port;
    neighbour_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    status[i] = connect(client_socket, (struct sockaddr *)&neighbour_address, sizeof(neighbour_address));
    if (status[i] > -1)
    {
        send(client_socket[i], msg2, strlen(msg2), 0);
        flag[i] = true;
    }
}