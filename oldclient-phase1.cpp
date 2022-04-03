#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>

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

	int server_socket, new_client, client_msg_len;
	struct sockaddr_in address;
	int yes = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {};

	std::cout << "Preprocessing done" << std::endl;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	// fcntl(server_socket, F_SETFL, O_NONBLOCK);
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(int));

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	std::cout << "Socket Created, port freed" << std::endl;
	bind(server_socket, (struct sockaddr *)&address, sizeof(address));
	int client_socket[num_neighbour] = {};
	int status[num_neighbour] = {};
	bool flag[num_neighbour] = {};
	bool flag_0[num_neighbour] = {};
	struct sockaddr_in neighbour_address[num_neighbour];
	bool change = false;
	int count = 0;
	std::cout << "Starting with the loop" << std::endl;
	while (true)
	{
		if (count < num_neighbour)
		{
			if (listen(server_socket, 10) != -1)
			{
				std::cout << "Listening done" << std::endl;

				new_client = accept(server_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen);
				std::cout << "Accepted, valid or not" << std::endl;

				if (new_client > -1)
				{
					std::cout << "Accepted and valid" << std::endl;
					client_msg_len = read(new_client, buffer, 1024);
					if (client_msg_len > 0)
					{
						printf("%s\n", buffer);
						flag_0[buffer[0] - '0'] = true;
						count++;
					}
				}
				change = true;
			}
		}

		for (int i = 0; i < num_neighbour; i++)
		{
			std::cout << "Action as a client started" << std::endl;

			if (flag[i])
			{
				continue;
			}
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
		{
			break;
		}
	}

	return 0;
}
