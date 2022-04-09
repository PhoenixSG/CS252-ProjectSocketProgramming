#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <thread>

void tokenize(std::string &str, char delim, std::vector<std::string> &out)
{
	size_t start;
	size_t end = 0;

	while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
	{
		end = str.find(delim, start);
		out.push_back(str.substr(start, end - start));
	}
}

void print_file_names(char **argv)
{

	std::string file = argv[1];
	std::string path = argv[2];
	std::string commd = "cd " + path + ";" + "ls -p | grep -v /";
	system(commd.c_str());
}

int main(int argc, char **argv)
{
	////////////////////Processing

	print_file_names(argv);

	std::string file = argv[1];

	std::ifstream read_file(file);

	std::string line;

	getline(read_file, line);
	line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
	std::vector<std::string> out;
	tokenize(line, ' ', out);

	int S_NO = atoi(out[2].c_str());
	int PORT = atoi(out[1].c_str());
	int ID = atoi(out[0].c_str());
	// printf("I am %d on port %d with id %d\n", ID, PORT, S_NO);
	getline(read_file, line);
	line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

	int num_neighbour = atoi(line.c_str());

	getline(read_file, line);
	line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
	std::vector<std::string> out2;
	tokenize(line, ' ', out2);
	std::vector<int> neighbour_client_number;
	std::vector<int> neighbour_client_port;

	for (int i = 0; i < num_neighbour; i++)
	{
		neighbour_client_number.push_back(atoi(out2[2 * i].c_str()));
		neighbour_client_port.push_back(atoi(out2[2 * i + 1].c_str()));
	}

	getline(read_file, line);
	line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

	int num_files = atoi(line.c_str());

	std::vector<int> files_to_download;

	for (int i = 0; i < num_files; i++)
	{
		getline(read_file, line);
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		files_to_download.push_back(atoi(line.c_str()));
	}

	read_file.close();

	/////////////////////////////////////////////////////////////////////////////////

	int client_or_server = fork();

	if (client_or_server == 0)
	{
		// client process

		// std::cout << "Client " << S_NO << " starting up. Num neighbours = "<<num_neighbour << std::endl;

		struct sockaddr_in neighbour_address[num_neighbour];
		int client_socket[num_neighbour];
		int status[num_neighbour];

		for (int i = 0; i < num_neighbour; i++)
		{

			int valread;
			std::string msg = std::to_string(S_NO) + " You are " + std::to_string(neighbour_client_number[i]) + ". I am " + std::to_string(S_NO) + ", on port " + std::to_string(PORT) + ",with ID " + std::to_string(ID);
			const char *msg2 = msg.c_str();
			char buffer[1024] = {0};
			client_socket[i] = socket(AF_INET, SOCK_STREAM, 0);

			neighbour_address[i].sin_family = AF_INET;
			neighbour_address[i].sin_port = htons(neighbour_client_port[i]);
			neighbour_address[i].sin_addr.s_addr = inet_addr("127.0.0.1");
			memset(&(neighbour_address[i].sin_zero), '\0', 8); // zero the rest of the struct
			// printf("Client attempting connect to port %d \n", neighbour_client_port[i]);

			int status = 0;
			while (status == 0)
			{
				status = connect(client_socket[i], (struct sockaddr *)&neighbour_address[i], sizeof(neighbour_address[i]));
				if (status > -1)
				{
					printf("Client Connected to port %d \n", neighbour_client_port[i]);
					// int len_of_msg_remaining = strlen(msg2);
					// // int sent_till_now = 0;
					// int sent_status;
					// while (len_of_msg_remaining > 0)
					// {
					// 	sent_status = send(client_socket[i], msg2, len_of_msg_remaining, 0);
					// 	msg2 += sent_status;
					// 	len_of_msg_remaining -= sent_status;
					// 	// sent_till_now+=sent_status;
					// }
					send(client_socket[i], msg2, strlen(msg2), 0);
					printf("Client SENT data to port %d \n", neighbour_client_port[i]);
					close(client_socket[i]);
				}
			}
		}
	}
	else
	{
		int opt = 1;
		int master_socket, addrlen, new_socket, client_socket[30],
			max_clients = 30, activity, i, valread, sd;
		int max_sd;
		struct sockaddr_in address;

		char buffer[1025]; // data buffer of 1K

		// set of socket descriptors
		fd_set readfds;

		// a message
		// char *message = "Hello";

		// initialise all client_socket[] to 0 so not checked
		for (i = 0; i < max_clients; i++)
		{
			client_socket[i] = 0;
		}

		// create a master socket
		if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		{
			perror("socket failed");
			exit(EXIT_FAILURE);
		}

		// set master socket to allow multiple connections ,
		// this is just a good habit, it will work without this
		if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
					   sizeof(opt)) < 0)
		{
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}

		// type of socket created
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(PORT);

		// bind the socket to localhost port 8888
		if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
		{
			perror("bind failed");
			exit(EXIT_FAILURE);
		}
		printf("Listener on port %d \n", PORT);

		// try to specify maximum of 3 pending connections for the master socket
		if (listen(master_socket, 3) < 0)
		{
			perror("listen");
			exit(EXIT_FAILURE);
		}

		// accept the incoming connection
		addrlen = sizeof(address);
		puts("Waiting for connections ...");

		while (1)
		{
			// clear the socket set
			FD_ZERO(&readfds);

			// add master socket to set
			FD_SET(master_socket, &readfds);
			max_sd = master_socket;

			// add child sockets to set
			for (i = 0; i < max_clients; i++)
			{
				// socket descriptor
				sd = client_socket[i];

				// if valid socket descriptor then add to read list
				if (sd > 0)
					FD_SET(sd, &readfds);

				// highest file descriptor number, need it for the select function
				if (sd > max_sd)
					max_sd = sd;
			}

			// wait for an activity on one of the sockets , timeout is NULL ,
			// so wait indefinitely
			activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

			if ((activity < 0) && (errno != EINTR))
			{
				printf("select error");
			}

			// If something happened on the master socket ,
			// then its an incoming connection
			if (FD_ISSET(master_socket, &readfds))
			{
				if ((new_socket = accept(master_socket,
										 (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
				{
					perror("accept");
					exit(EXIT_FAILURE);
				}

				// inform user of socket number - used in send and receive commands
				printf("New connection , socket fd is %d , ip is : %s , port : %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

				// // send new connection greeting message
				// if (send(new_socket, message, strlen(message), 0) != strlen(message))
				// {
				// 	perror("send");
				// }

				// puts("Welcome message sent successfully");

				// add new socket to array of sockets
				for (i = 0; i < max_clients; i++)
				{
					// if position is empty
					if (client_socket[i] == 0)
					{
						client_socket[i] = new_socket;
						printf("Adding to list of sockets as %d\n", i);

						break;
					}
				}
			}

			// else its some IO operation on some other socket
			for (i = 0; i < max_clients; i++)
			{
				sd = client_socket[i];

				if (FD_ISSET(sd, &readfds))
				{
					// Check if it was for closing , and also read the
					// incoming message
					if ((valread = read(sd, buffer, 1024)) == 0)
					{
						// Somebody disconnected , get his details and print
						getpeername(sd, (struct sockaddr *)&address,
									(socklen_t *)&addrlen);
						printf("Host disconnected , ip %s , port %d \n",
							   inet_ntoa(address.sin_addr), ntohs(address.sin_port));

						// Close the socket and mark as 0 in list for reuse
						close(sd);
						client_socket[i] = 0;
					}

					// Echo back the message that came in
					else
					{
						printf("Received %d bits of data\n", valread);
						// set the string terminating NULL byte on the end
						// of the data read
						buffer[valread] = '\0';
						printf("Valread = %d\n", valread);
						printf("Buffer = %s\n", buffer);
						send(sd, buffer, strlen(buffer), 0);
					}
				}
			}
		}
	}

	return 0;
}