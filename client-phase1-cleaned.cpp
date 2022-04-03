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
std::mutex mu;
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

void server(int port, int num_neighbour)
{

	int valread;

	char buffer[1024] = {0};
	// Creating socket file descriptor

	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	int yes = 1;
	// std::cout << "Server "<< " made with fd as " << server_socket << "and port " << port << std::endl;

	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(yes));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	bind(server_socket, (struct sockaddr *)&address, sizeof(address));
	int count = 0;

	std::string msg = "nahi mila";
		const char *msg2 = msg.c_str();


	while (count < num_neighbour)
	{
		if (listen(server_socket, 10) != -1)
		{

			// std::cout << "Server " << " listening on port " << port << std::endl;

			int new_client = accept(server_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen);
			// std::cout << "Server accept returned " << new_client << std::endl;

			if (new_client > -1)
			{
				// std::cout << "Server accepted a connection " << std::endl;
// 
				int client_msg_len = read(new_client, buffer, 1024);
				if (client_msg_len > 48)
				{
					// std::cout << "Server received a message " << std::endl;
					mu.lock();
					printf("%s\n", buffer);
					mu.unlock();
					// printf("Amount of text received-%d\n", client_msg_len);
					count++;
				}
				else{
					send(new_client,msg2,strlen(msg2),0);
				}
			}
		}
	}
}

void client(int S_NO, int num_neighbour, std::vector<int> &neighbour_client_port, std::vector<int> &neighbour_client_number, int PORT, int ID)
{

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
		// printf("Client attempting connect to port %d \n", neighbour_client_port[i]);

		int status = 0;
		while (status == 0)
		{
			status = connect(client_socket[i], (struct sockaddr *)&neighbour_address[i], sizeof(neighbour_address[i]));
			if (status > -1)
			{
				// printf("Client Connected to port %d \n", neighbour_client_port[i]);

				int send_status = send(client_socket[i], msg2, strlen(msg2), 0);

				if (send_status > 48)
				{
					status = 1;
					// printf("Hello message sent\n");
				}
				else{
					read(client_socket[i], buffer, 1024);
					status = 0;
				}
			}
		}
	}
}

int main(int argc, char **argv)
{
	////////////////////Processing

	// std::cout << "Starting up" << std::endl;

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

	/////////////////////////////////////////////////////////////

	std::thread t1(server, PORT, num_neighbour);

	// server(server_socket, change, address, num_neighbour);

	std::thread t2(client, S_NO, num_neighbour, std::ref(neighbour_client_port),
				   std::ref(neighbour_client_number), PORT, ID);

	// client(S_NO, change, num_neighbour, neighbour_client_port,
	// neighbour_client_number, PORT, ID);

	t1.join();
	t2.join();

	return 0;
}