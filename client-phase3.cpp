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
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <chrono>

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
	std::string commd = "ls -1vp " + path + " | grep -v /";
	system(commd.c_str());
}

std::vector<std::string> mod_get_file_names(std::string msg)
{

	auto otp = msg.substr(msg.find(";"));
	otp = &otp[1];
	std::vector<std::string> r1;
	tokenize(otp, '\n', r1);
	std::vector<std::string> retval;
	std::vector<std::string> temp;
	for (auto x : r1)
	{
		tokenize(x, '#', temp);
		retval.push_back(temp[0]);
		temp.clear();
	}
	return retval;
}

std::map<std::string, off_t> file_size_map(std::string msg)
{

	std::map<std::string, off_t> retval;
	auto otp = msg.substr(msg.find(";"));
	otp = &otp[1];
	std::vector<std::string> r1;
	tokenize(otp, '\n', r1);
	std::vector<std::string> temp;
	for (auto x : r1)
	{
		tokenize(x, '#', temp);
		retval[temp[0]] = off_t(std::stoi(temp[1]));
		temp.clear();
	}

	return retval;
}

std::string GetStdoutFromCommand(std::string cmd)
{

	std::string data;
	FILE *stream;

	char buffer[1000];

	stream = popen(cmd.c_str(), "r");
	if (stream)
	{
		while (!feof(stream))
			if (fgets(buffer, 1000, stream) != NULL)
				data.append(buffer);
		pclose(stream);
	}
	return data;
}
void get_file_len(std::string file_to_send, off_t &len)
{

	struct stat file_stat;

	auto fd = open(file_to_send.c_str(), O_RDONLY);
	if (fd == -1)
	{
		std::cout << "\n"
				  << file_to_send << "<- Send this" << std::endl;
		puts("Failed to open file");
		// puts(GetStdoutFromCommand("pwd").c_str());
		system(std::string("mkdir -p Fail" + file_to_send).c_str());
		close(fd);
		return;
	}
	if (fstat(fd, &file_stat) < 0)
	{
		std::cout << file_to_send;
		puts("Failed to do file stat");
		close(fd);
		return;
	}
	len = file_stat.st_size;
	close(fd);
}

std::string repackage(std::string file_names, std::string path)
{

	std::vector<std::string> r;
	tokenize(file_names, '\n', r);
	off_t len = 0;
	std::string retval = "";
	for (auto x : r)
	{
		get_file_len(path + x, len);
		retval += (x + "#" + std::to_string(int(len)) + "\n");
	}
	return retval;
}

std::string get_hash(std::string file_name)
{

	std::string x = GetStdoutFromCommand("md5sum " + file_name);
	std::vector<std::string> temp;
	tokenize(x, ' ', temp);
	return temp[0];
}

std::string process(std::string const &s)
{
	std::string::size_type pos = s.find(';');
	if (pos != std::string::npos)
	{
		return s.substr(0, pos);
	}
	else
	{
		return s;
	}
}

std::vector<std::string> intersection(std::vector<std::string> &v1, std::vector<std::string> &v2)
{

	std::vector<std::string> v3;
	std::sort(v1.begin(), v1.end());
	std::sort(v2.begin(), v2.end());
	std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v3));

	return v3;
}

std::string piece_together(std::vector<std::string> v, char delim)
{
	std::string retval = "";
	if (v.size() == 0)
		return retval;
	for (int i = 0; i < v.size() - 1; ++i)
		retval += (v[i] + delim);
	retval += v[v.size() - 1];
	return retval;
}
/*
int index (int* client_socket, int x){
	int retval = 0;
	for(;retval < 30; ++retval)
		if(client_socket[retval] == x)
			return retval;


}
*/
bool all_true(bool *arr, int n)
{
	for (int i = 0; i < n; ++i)
		if (!arr[i])
			return false;
	return true;
}
/*
void clean_map (std::map<int, std::vector<std::string>> &server_map){
	for(auto x : server_map){
		auto y = x.second;
		if(y.size() == 0)
			server_map.erase(x.first);
	}


}*/

void client(int S_NO, int num_neighbour, std::vector<int> &neighbour_client_port, std::vector<int> &neighbour_client_number, int PORT, int ID, std::string path)
{
	struct sockaddr_in neighbour_address[num_neighbour];
	int client_socket[num_neighbour];
	int status[num_neighbour];
	std::string msg;
	for (int i = 0; i < num_neighbour; i++)
	{

		int valread;
		msg = "Connected to " + std::to_string(ID) + " with unique-ID " + std::to_string(S_NO) + " on port " + std::to_string(PORT) + ";" + repackage(GetStdoutFromCommand("ls -1vp " + path + " | grep -v /"), path);
		const char *msg2 = msg.c_str();
		char buffer[BUFSIZ];
		bzero(buffer, BUFSIZ);
		client_socket[i] = socket(AF_INET, SOCK_STREAM, 0);

		neighbour_address[i].sin_family = AF_INET;
		neighbour_address[i].sin_port = htons(neighbour_client_port[i]);
		neighbour_address[i].sin_addr.s_addr = inet_addr("127.0.0.1");
		memset(&(neighbour_address[i].sin_zero), '\0', 8);

		int status = -1;
		while (status == -1)
		{
			status = connect(client_socket[i], (struct sockaddr *)&neighbour_address[i], sizeof(neighbour_address[i]));
			if (status > -1)
			{
				send(client_socket[i], msg2, strlen(msg2), 0);
				// close(client_socket[i]);
			}
		}
	}

	// std::cout << msg ;
	// receive loop
	std::map<int, std::vector<std::string>> server_map;
	for (int i = 0; i < num_neighbour; i++)
	{
		char buffer2[1000] = {'\0'};

		recv(client_socket[i], buffer2, 1000, 0);
		std::string m = buffer2;
		std::vector<std::string> x;
		int c = m[0] - '0';
		m.erase(m.begin());
		tokenize(m, '\n', x);
		auto l = piece_together(x, ' ');

		server_map[c] = x;

		std::cout << c << ", " << l << std::endl;
		// for(auto y:server_map[c]){
		// 	std::cout<<y<<std::endl;
		// }
	}
	// server_map stores the # of files to be sent to server[i]
	//  send loop

	int cnt = 0;

	/// int array of num_neighbours.
	std::map<int, int> file_sent_status;
	int successfully_sent_counter = 0;
	cnt = 0;
	for (auto x : server_map)
	{
		file_sent_status[x.first] = 0;
		if (x.second.size() == 0)
		{
			close(client_socket[cnt]);
			successfully_sent_counter++;
		}
		cnt++;
	}
	std::cout << "Beginning to send, success counter " << successfully_sent_counter << " Num Neigh " << num_neighbour << std::endl;
	while (successfully_sent_counter < num_neighbour)
	{
		cnt = 0;
		for (auto x : server_map)
		{
			ssize_t len;
			int fd, offset = 0, sent_bytes = 0;
			off_t remain_data;
			if (x.second.size() == file_sent_status[x.first])
			{
				++cnt;
				continue;
			}
			std::string y = x.second[file_sent_status[x.first]];
			get_file_len(path + y, remain_data);
			fd = open((path + y).c_str(), O_RDONLY);

			// std::cout << "\n\nOpen status" << fd << std::endl;
			// std::cout << "File sent status" << file_sent_status[x.first] << std::endl;
			// std::cout << "Successfully sent counter" << successfully_sent_counter << std::endl;
			// std::cout << path + y << std::endl;
			// std::cout << "Remaining data- " << remain_data << std::endl;
			// std::cout << "Client socket number- " << cnt << std::endl;

			auto start = std::chrono::system_clock::now(); // timer begin
			auto now = start;
			std::chrono::duration<double> diff = now - start;
			double time;
			// while (true)
			// {
			while (remain_data > 0)
			{
				sent_bytes = sendfile(client_socket[cnt], fd, (off_t *)&offset, remain_data);
				if (sent_bytes < 0)
				{
					std::cout << "Client could not send" << std::endl;
					break;
				}
				else if(sent_bytes == 0)
				{
					// std::cout << "Zero data sent on " <<client_socket[cnt]<< std::endl;
					break;
				}
				remain_data -= sent_bytes;
				std::cout << "Sent " << sent_bytes << " bytes from file's data, offset is now : " << offset << " and remaining data = " << remain_data << "\n";
			}
			// 	now = std::chrono::system_clock::now(); // timer end
			// 	diff = now - start;
			// 	time = diff.count();
			// 	if (time > 0.1)
			// 	{
			// 		break;
			// 	}
			// }
			if (remain_data == 0)
			{
				file_sent_status[x.first]++;
				std::cout << "Sent a FILE " << y << std::endl;
			}
			if (file_sent_status[x.first] == x.second.size())
			{
				successfully_sent_counter++;
				std::cout << "Sent all to one Server " << successfully_sent_counter << std::endl;
			}
			++cnt;
			close(fd);
		}
	}

	std::cout << S_NO << "    DONEDONE" << std::endl;
}

void display(std::vector<int> &neighbour_client_number)
{
	for (auto x : neighbour_client_number)
		std::cout << x << " ";
	std::cout << std::endl;
}

// void display (std::vector<std::string> arr[4]){
// 	for(int i = 0; i < 4; ++i){
// 		for(auto x : arr[i])
// 			std::cout << x << " ";
// 		std::cout << "; ";
// 	}
// 	std::cout << "\nFile client map printed\n";
// }

void display(std::map<std::string, int> m)
{
	for (auto x : m)
		std::cout << "(" << x.first << ", " << x.second << "), ";
	std::cout << std::endl;
}

void server(int PORT, std::vector<std::string> files_to_download, int num_neighbours,
			std::vector<int> &neighbour_client_number, int ID, std::string path)
{
	int opt = 1;
	int master_socket, addrlen, new_socket, client_socket[30], max_clients = 30, activity, valread, sd;
	int max_sd, num_connections = 0;
	struct sockaddr_in address;

	char buffer[BUFSIZ];
	bzero(buffer, BUFSIZ);

	fd_set readfds;
	for (int i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}

	if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(master_socket, 10) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	addrlen = sizeof(address);

	std::map<std::string, int> file_map;
	std::map<std::string, int> file_map_no;
	std::map<int, std::string> m;
	std::map<std::string, off_t> file_sz_map;
	std::vector<std::string> file_client_map[30];

	std::map<int, int> client_socket_ordered;
	while (num_connections < num_neighbours)
	{
		FD_ZERO(&readfds);
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;
		for (int i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];
			if (sd > 0)
				FD_SET(sd, &readfds);
			if (sd > max_sd)
				max_sd = sd;
		}
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

		if ((activity < 0) && (errno != EINTR))
		{
			printf("select error");
		}
		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket,
									 (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}

			for (int i = 0; i < max_clients; i++)
			{
				if (client_socket[i] == 0)
				{
					client_socket[i] = new_socket;
					break;
				}
			}
		}

		for (int i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];

			if (FD_ISSET(sd, &readfds))
			{
				if ((valread = read(sd, buffer, BUFSIZ)) == 0)
				{
					getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
					// close(sd);
					// client_socket[i] = 0;
				}

				else
				{
					++num_connections;
					buffer[valread] = '\0';
					std::string str = buffer;
					std::string str2 = buffer;

					std::istringstream s(str);
					std::string temp, word, word2;

					s >> temp >> temp >> word2 >> temp >> temp >> word;

					auto file_names = mod_get_file_names(str2);

					auto x = file_size_map(str2);
					file_sz_map.insert(x.begin(), x.end());
					auto files_available = intersection(file_names, files_to_download);
					// file_client_map[std::stoi(word2)-1] = files_available;

					int w = std::stoi(word), w2 = std::stoi(word2);
					std::cout << "Socket number of no. " << w2 << " with ID " << w << " is " << sd << std::endl;
					client_socket_ordered[w2] = sd;
					for (auto x : files_available)
					{
						if (file_map.find(x) == file_map.end())
						{
							file_map[x] = w;
							file_map_no[x] = w2;
						}
						else
						{
							// file_map[x] = std::min(w, file_map[x]);
							// file_map_no[x] =
							if (w < file_map[x])
							{
								file_map[x] = w;
								file_map_no[x] = w2;
							}
						}
					}

					auto prnt = process(str) + "\n";

					m[atoi(word2.c_str())] = prnt;
				}
			}
		}
	}

	for (auto x : file_map_no)
	{
		file_client_map[x.second].push_back(x.first);
	}
	// std::cout << ID << "     "; display(file_client_map);
	// send loop
	int sz = neighbour_client_number.size();
	bool all_sent[sz];

	for (int i = 0; i < sz; ++i)
		all_sent[i] = false;
	// std::cout << "Node number " << ID << " is still alive1\n";
	while (!all_true(all_sent, sz))
	{
		int i = 0;
		for (auto current_socket : client_socket_ordered)
		{
			ssize_t by_sent;
			if (!all_sent[i])
			{
				std::string blah = std::to_string(ID) + piece_together(file_client_map[neighbour_client_number[i]], '\n');
				const char *x = blah.c_str();
				by_sent = send(current_socket.second, x, strlen(x), 0);
				if (by_sent != -1)
				{
					all_sent[i] = true;
				}
			}
			i++;
		}
	}
	for (auto x : files_to_download)
	{
		if (file_map.find(x) == file_map.end())
		{
			file_map[x] = 0;
		}
	}

	for (auto x : m)
	{
		std::cout << x.second;
	}

	for (auto i = file_map.begin(); i != file_map.end(); ++i)
	{
		if (i->second == 0)
		{
			std::string s1 = ("Found " + i->first + " at " + std::to_string(i->second) + " with MD5 0 at depth 0\n");
			std::cout << s1;
		}
		else
		{
			std::string s1 = ("Found " + i->first + " at " + std::to_string(i->second) + " with MD5 0 at depth 1\n");
			std::cout << s1;
		}
	}
	// std::cout << "Node number " << ID << " is still alive2\n";
	std::string rmdir = "rm -rf " + path + "Downloaded";
	system(rmdir.c_str());
	std::string mkdir = "mkdir -p " + path + "Downloaded";
	system(mkdir.c_str());
	// receive loop
	std::map<int, int> file_rec_status;
	int num_received_success = 0;

	for (auto current_socket : client_socket_ordered)
	{
		if (file_client_map[current_socket.first].size() == 0)
		{
			num_received_success++;
		}
		else
		{
			std::cout << "FILE CLIENT MAP BEGIN for " <<current_socket.first<< std::endl;
			for (auto files : file_client_map[current_socket.first])
			{
				std::cout << files << std::endl;
			}
			std::cout << "FILE CLIENT MAP END for " <<current_socket.first<< std::endl;
		}
		file_rec_status[current_socket.first] = 0;
	}
	std::set<int> sockets_alive;
	fd_set connected_fds;

	FD_ZERO(&connected_fds);
	max_sd = 0;
	for (auto current_socket : client_socket_ordered)
	{
		sd = current_socket.second;
		if (sd > 0)
		{
			sockets_alive.insert(sd);
			std::cout << "Inserting "<<sd<<" to the list" << std::endl;

			FD_SET(sd, &connected_fds);
		}
		else
		{
			std::string mkdir = "mkdir -p SOCKET_CLOSED";
			system(mkdir.c_str());
		}
		if (sd > max_sd)
			max_sd = sd;
	}
	assert(sockets_alive.size() == num_neighbours);
	const fd_set total_set = connected_fds;
	
	struct timeval time_out;
	time_out.tv_sec = 1;
	while (num_received_success < num_neighbours)
	{
		std::cout << "I am alive" << std::endl;
		connected_fds = total_set;
		activity = select(max_sd + 1, &connected_fds, NULL, NULL, &time_out);
		std::cout << "I am seeing some activity" << std::endl;

		if ((activity < 0) && (errno != EINTR))
		{
			std::cout << "select error" << std::endl;
			continue;
		}

		for (auto current_socket : client_socket_ordered)
		{
			if (FD_ISSET(current_socket.second, &connected_fds))
			{
				std::cout << "Message from " << current_socket.first << " on socket " << current_socket.second << std::endl;

				if (file_rec_status[current_socket.first] == file_client_map[current_socket.first].size())
				{
					break;
				}
				std::string file_name = file_client_map[current_socket.first][file_rec_status[current_socket.first]];
				// receiving data.
				/// maintain a 2d array which keeps track of what is received. In order receiving.
				// Variable which maintains the file number till where we have received, for each client.
				// int array of num_neighbour
				std::cout << "ACTIVITY ON- " << current_socket.first << " with file" << file_name << std::endl;
				std::cout << "Current Status " << file_rec_status[current_socket.first] << std::endl;
				std::cout << "Total Required " << file_client_map[current_socket.first].size() << std::endl;
				ssize_t len;
				int sent_bytes = 0;
				off_t remain_data;
				FILE *received_file = fopen((path + "Downloaded/" + file_name).c_str(), "w");

				remain_data = file_sz_map[file_name];
				std::cout << "Size of file " << remain_data << std::endl;

				while (remain_data > 0)
				{
					len = recv(current_socket.second, buffer, BUFSIZ, 0);
					if (len < 0)
					{
						std::string mkdir = "mkdir -p " + path + "Error" + std::to_string(ID) + file_name;
						system(mkdir.c_str());
						fclose(received_file);
						std::cout << "Error while receiving " << file_name << std::endl;
						break;
					}
					if (len == 0)
					{
						std::string mkdir = "mkdir -p " + path + "AHHHHHHHHH" + std::to_string(ID) + file_name;
						system(mkdir.c_str());
						fclose(received_file);
						std::cout << "Error while receiving " << file_name << std::endl;
						break;
					}
					fwrite(buffer, sizeof(char), len, received_file);
					bzero(buffer, BUFSIZ);
					remain_data -= len;
					std::cout << "Received " << len << " bytes and remaining -> " << remain_data << " bytes\n";
				}
				if (remain_data == 0)
				{
					file_rec_status[current_socket.first]++;
					printf("Received a file\n\n");
				}
				if (file_rec_status[current_socket.first] == file_client_map[current_socket.first].size())
				{
					num_received_success++;
					printf("One server done %d\n\n", current_socket.first);
				}
				fclose(received_file);
			}
		}
	}

	std::cout << PORT << "        DONEDONEDONEDONE" << std::endl
			  << std::endl;
	// std::cout << "Node number " << ID << " is still alive\n";
}

int main(int argc, char **argv)
{

	////////////////////Processing

	print_file_names(argv);

	std::string file = argv[1];
	std::string path = argv[2];
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

	std::vector<std::string> files_to_download;

	for (int i = 0; i < num_files; i++)
	{
		getline(read_file, line);
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		files_to_download.push_back(line);
	}

	read_file.close();

	/////////////////////////////////////////////////////////////////////////////////

	std::thread t1(server, PORT, std::ref(files_to_download), num_neighbour, std::ref(neighbour_client_number), ID, std::ref(path));

	std::thread t2(client, S_NO, num_neighbour, std::ref(neighbour_client_port),
				   std::ref(neighbour_client_number), PORT, ID, std::ref(path));

	t1.join();
	t2.join();

	return 0;
}
