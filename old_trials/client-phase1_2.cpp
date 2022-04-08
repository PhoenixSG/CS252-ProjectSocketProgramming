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

int main(int argc, char **argv)
{
	std::string file = argv[1];
	std::string path = argv[2];
	std::string commd = "cd " + path + ";" + "ls -p | grep -v /";
	system(commd.c_str());

	std::ifstream read_file(file);

	std::string line;

	getline(read_file, line);
	line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
	std::vector<std::string> out;
	tokenize(line, ' ', out);
	int S_NO = atoi(out[2].c_str());
	int PORT = atoi(out[1].c_str());
	int ID = atoi(out[0].c_str());

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

	////////////////////////////////////////////////////////////////////////////////////////////////////

	int server_socket, new_client, client_msg_len;
	struct sockaddr_in address;
	int yes = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {};

	// Creating socket file descriptor
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (server_socket < 0)
	{
		perror("socket() failed");
		exit(1);
	}

	int rc = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(int));

	if (rc < 0)
	{
		perror("setsockopt() failed");
		close(server_socket);
		exit(1);
	}

	rc = ioctl(server_socket, FIONBIO, &yes);

	if (rc < 0)
	{
		perror("ioctl() failed");
		close(server_socket);
		exit(1);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	bzero(&(address.sin_zero), 8);

	rc = bind(server_socket, (struct sockaddr *)&address, sizeof(address));

	if (rc < 0)
	{
		perror("bind() failed");
		close(server_socket);
		exit(-1);
	}

	int client_socket[num_neighbour] = {};
	int status[num_neighbour] = {};
	bool flag[num_neighbour] = {};
	bool flag_0[num_neighbour] = {};
	struct sockaddr_in neighbour_address[num_neighbour];
	bool change = false;
	int count = 0;

	struct pollfd fds[5];
	int nfds = 1, current_size;
	int timeout = 200*rand()/RAND_MAX;

	memset(fds, 0, sizeof(fds));
	fds[0].fd = server_socket;
	fds[0].events = POLLIN;

	while (true)
	{
		/*
		if(count < num_neighbour){
			if(listen(server_socket, 10) != -1){
				new_client = accept(server_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen);
				if (new_client > -1){
					client_msg_len = read(new_client, buffer, 1024);
					if(client_msg_len > 0){
						printf("%s\n", buffer);
						flag_0[buffer[0]-'0'] = true;
						count++;
					}
				}
				change = true;
			}

		}
		*/

		for (int i = 0; i < num_neighbour; i++)
		{
			if (flag[i])
				continue;

			change = true;
			std::string msg = std::to_string(S_NO) + "You are " + std::to_string(neighbour_client_number[i]) + ". I am " + std::to_string(S_NO) + ", on port " + std::to_string(PORT) + ",with ID " + std::to_string(ID);
			const char *msg2 = msg.c_str();
			client_socket[i] = socket(AF_INET, SOCK_STREAM, 0);

			neighbour_address[i].sin_family = AF_INET;
			neighbour_address[i].sin_port = neighbour_client_port[i];
			neighbour_address[i].sin_addr.s_addr = inet_addr("127.0.0.1");

			status[i] = connect(client_socket[i], (struct sockaddr *)&neighbour_address[i], sizeof(neighbour_address[i]));
			if (status[i] > -1)
			{
				send(client_socket[i], msg2, strlen(msg2), 0);
				flag[i] = true;
			}
		}
		if (!change)
			break;

		if (poll(fds, nfds, timeout) > 0)
		{
			current_size = nfds;
			for (int i = 0; i < current_size; ++i)
			{
				if (fds[i].revents == 0)
					continue;
				if (fds[i].fd == server_socket)
				{

					do
					{
						new_client = accept(server_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen);
						printf("  New incoming connection - %d\n", new_client);
						if (new_client > -1)
						{
							client_msg_len = read(new_client, buffer, 1024);
							if (client_msg_len > 0)
							{
								printf("%s\n", buffer);
								flag_0[buffer[0] - '0'] = true;
								count++;
							}
						}
						change = true;
						fds[nfds].fd = new_client;
						fds[nfds].events = POLLIN;
						nfds++;
					} while (new_client != -1);
				}
			}
		}
	}

	return 0;
}