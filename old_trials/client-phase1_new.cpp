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

	fd_set master;				   // master file descriptor list
	fd_set read_fds;			   // temp file descriptor list for select()
	struct sockaddr_in myaddr;	   // server address
	struct sockaddr_in remoteaddr; // client address
	int fdmax;					   // maximum file descriptor number
	int listener;				   // listening socket descriptor
	int newfd;					   // newly accept()ed socket descriptor
	char buf[256];				   // buffer for client data
	int nbytes;
	struct timeval time_value;
	time_value.tv_sec = 0;
	time_value.tv_usec = 100000;
	


	int yes = 1; // for setsockopt() SO_REUSEADDR, below
	int addrlen;
	
	int i, j;
	FD_ZERO(&master); // clear the master and temp sets
	FD_ZERO(&read_fds);
	// get the listener
	if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}
	// lose the pesky "address already in use" error message
	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes,
				   sizeof(int)) == -1)
	{
		perror("setsockopt");
		exit(1);
	}

	// bind
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = INADDR_ANY;
	myaddr.sin_port = htons(PORT);

	memset(&(myaddr.sin_zero), '\0', 8);

	if (bind(listener, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1)
	{
		perror("bind");
		exit(1);
	}
	// listen
	if (listen(listener, 10) == -1)
	{
		perror("listen");
		exit(1);
	}
	// add the listener to the master set
	FD_SET(listener, &master);
	// keep track of the biggest file descriptor
	fdmax = listener; // so far, it’s this one
					  // main loop

	int server_socket, new_client, client_msg_len;
	struct sockaddr_in address;
	// int yes = 1;
	// int addrlen = sizeof(address);
	char buffer[1024] = {};

	std::cout << "Preprocessing done" << std::endl;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(server_socket, F_SETFL, O_NONBLOCK);
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
			read_fds = master; // copy it
			if (select(fdmax + 1, &read_fds, NULL, NULL, &time_value) == -1)
			{
				perror("select");
				continue;
			}
			// run through the existing connections looking for data to read
			for (i = 0; i <= fdmax; i++)
			{
				if (FD_ISSET(i, &read_fds))
				{ // we got one!!
					if (i == listener)
					{
						// handle new connections
						addrlen = sizeof(remoteaddr);
						if ((newfd = accept(listener, (struct sockaddr *)&remoteaddr, (socklen_t *)&addrlen)) == -1)
						{
							perror("accept");
						}
						else
						{
							FD_SET(newfd, &master); // add to master set
							if (newfd > fdmax)
							{ // keep track of the maximum
								fdmax = newfd;
							}
							printf("selectserver: new connection from %s on "
								   "socket %d\n",
								   inet_ntoa(remoteaddr.sin_addr), newfd);
						}
					}
					else
					{

						// handle data from a client
						if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0)
						{
							// got error or connection closed by client
							if (nbytes == 0)
							{
								// connection closed
								printf("selectserver: socket %d hung up\n", i);
							}
							else
							{
								perror("recv");
							}
							close(i);			// bye!
							FD_CLR(i, &master); // remove from master set
						}
						else
						{
							std::cout << buf << std::endl;
							count++;
							// we got some data from a client
							// for (j = 0; j <= fdmax; j++)
							// {
							//     // send to everyone!
							//     if (FD_ISSET(j, &master))
							//     {
							//         // except the listener and ourselves
							//         if (j != listener && j != i)
							//         {
							//             if (send(j, buf, nbytes, 0) == -1)
							//             {
							//                 perror("send");
							//             }
							//         }
							//     }
							// }
						}
					} // it’s SO UGLY!
				}
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

			std::cout << "Client socket made" << std::endl;
			status[i] = connect(client_socket[i], (struct sockaddr *)&neighbour_address[i], sizeof(neighbour_address[i]));
			if (status[i] > -1)
			{
				std::cout << "Client connected, sending data" << std::endl;

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
