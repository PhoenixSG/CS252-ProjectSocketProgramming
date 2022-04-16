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
#include <fstream>

bool contains_char(std::string s, char c)
{
	if (s.find(c) != std::string::npos)
		return true;
	else
		return false;
}

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
		// std::cout << "\n"<< file_to_send << "<- Send this" << std::endl;
		// puts("Failed to open file");
		// puts(GetStdoutFromCommand("pwd").c_str());
		// system(std::string("mkdir -p Fail" + file_to_send).c_str());
		close(fd);
		return;
	}
	if (fstat(fd, &file_stat) < 0)
	{
		// std::cout << file_to_send;
		// puts("Failed to do file stat");
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
std::vector<std::string> set_diff(std::vector<std::string> v1,
								  std::vector<std::string> v2)
{
	std::vector<std::string> v3;

	std::sort(v1.begin(), v1.end());
	std::sort(v2.begin(), v2.end());

	std::set_difference(v1.begin(), v1.end(),
						v2.begin(), v2.end(),
						back_inserter(v3));
	return v3;
}

std::vector<std::string> intersection(std::vector<std::string> &v1, std::vector<std::string> &v2)
{

	std::vector<std::string> v3;
	std::sort(v1.begin(), v1.end());
	std::sort(v2.begin(), v2.end());
	std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(v3));

	return v3;
}
std::vector<std::string> intersection_set(std::vector<std::string> &v1, std::set<std::string> &v4)
{

	std::vector<std::string> v3;
	std::vector<std::string> v2;
	for(auto x:v4){
		v2.push_back(x);
	}
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
std::vector<std::string> get_keys(std::map<std::string, std::pair<int, int>> &fm)
{
	std::vector<std::string> retval;
	for (auto x : fm)
		retval.push_back(x.first);
	return retval;
}
/*
void clean_map (std::map<int, std::vector<std::string>> &server_map){
	for(auto x : server_map){
		auto y = x.second;
		if(y.size() == 0)
			server_map.erase(x.first);
	}


}*/

// void display(std::vector<int> &neighbour_client_number)
// {
// 	for (auto x : neighbour_client_number)
// 		std::cout << x << " ";
// 	std::cout << std::endl;
// }

// void display (std::vector<std::string> arr[4]){
// 	for(int i = 0; i < 4; ++i){
// 		for(auto x : arr[i])
// 			std::cout << x << " ";
// 		std::cout << "; ";
// 	}
// 	std::cout << "\nFile client map printed\n";
// }

// void display(std::map<std::string, int> m)
// {
// 	for (auto x : m)
// 		std::cout << "(" << x.first << ", " << x.second << "), ";
// 	std::cout << std::endl;
// }
std::string get_file_names_as_string(std::string msg)
{
	auto otp = msg.substr(msg.find(";"));
	otp = &otp[1];
	return otp;
}

std::pair<int, int> minimum(std::pair<int, int> lhs, std::pair<int, int> rhs)
{
	if (lhs.first < rhs.first)
	{
		return lhs;
	}
	else if (lhs.first == rhs.first && lhs.second < rhs.second)
	{
		return lhs;
	}
	else
	{
		return rhs;
	}
}
std::map<std::string, int> file_size_dist2;
bool done = false;
void extract_info(std::vector<std::string> svs,
				  std::set<std::string> &files_to_download,
				  std::map<std::string, std::pair<int, int>> &fm, std::vector<std::vector<int>> &svt)
{

	std::map<std::pair<int, int>, std::vector<std::string>> m;
	std::vector<std::string> temp;
	// std::map<std::string, std::pair<int, int>> fm;
	int sz = svs.size();
	// std::cout << svs[sz-1] << std::endl;
	// std::vector<std::string> temporary;
	// tokenize(svs[sz - 1], ',', temporary);
	// std::vector<int> tmp;
	// for (auto elem : temporary)
	// {
	// 	tmp.push_back(std::stoi(elem));
	// }
	// svt.push_back(tmp);
	int i = 0;
	for (; i < sz; ++i)
	{
		std::string x = svs[i];
		if (x.empty())
			continue;
		// if (contains_char(x, ',')){
		// 	std::cout << x << std::endl;
		// 	std::vector<std::string> temp;
		// 	tokenize(x,',',temp);
		// 	std::vector<int> tmp;
		// 	for(auto elem : temp){
		// 		tmp.push_back(std::stoi(elem));
		// 	}
		// 	svt.push_back(tmp);
		// 	//return;
		// }
		if (contains_char(x, ':'))
		{
			std::pair<int, int> p;
			p.first = std::stoi(x.substr(x.find(":") + 1));	 // p.first UID
			p.second = std::stoi(x.substr(x.find("|") + 1)); // p.second is the port
			int j = i + 1;
			for (; (j < sz) && !contains_char(svs[j], ':') && !contains_char(svs[j], ','); ++j)
			{
				std::vector<std::string> pieces;
				tokenize(svs[j], '#', pieces);
				file_size_dist2[pieces[0]] = std::stoi(pieces[1]);
				temp.push_back(pieces[0]);
			}
			m[p] = intersection_set(temp, files_to_download);
			for (std::string file : m[p])
			{
				if (fm.find(file) == fm.end())
				{
					fm[file] = p;
				}
				else
				{
					// std::cout << "Entered            ";
					// display(fm[file]);
					fm[file] = minimum(p, fm[file]);
					// std::cout << "Exited            ";
					// display(fm[file]);
				}
			}
			i = j - 1;
			temp.clear();
		}
	}
}
void client(int S_NO, int num_neighbour, std::vector<int> &neighbour_client_port, std::vector<int> &neighbour_client_number, int PORT, int ID, std::string path, std::vector<std::string> &files_to_download)
{
	struct sockaddr_in neighbour_address[num_neighbour];
	int client_socket[num_neighbour];
	int status[num_neighbour];
	std::string msg;
	char buffer[BUFSIZ];
	bzero(buffer, BUFSIZ);
	for (int i = 0; i < num_neighbour; i++)
	{

		int valread;
		msg = "Connected to " + std::to_string(ID) + " with unique-ID " + std::to_string(S_NO) + " on port " + std::to_string(PORT) + ";" + repackage(GetStdoutFromCommand("ls -1vp " + path + " | grep -v /"), path);
		const char *msg2 = msg.c_str();
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

		// std::cout << c << ", " << l << std::endl;
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
	// std::cout << "Beginning to send, success counter " << successfully_sent_counter << " Num Neigh " << num_neighbour << std::endl;
	bzero(buffer, BUFSIZ);
	while (successfully_sent_counter < num_neighbour)
	{
		cnt = 0;
		for (auto x : server_map)
		{
			ssize_t len;
			int offset = 0, sent_bytes = 0;
			off_t remain_data;
			if (x.second.size() == file_sent_status[x.first])
			{
				++cnt;
				continue;
			}
			std::string y = x.second[file_sent_status[x.first]];
			get_file_len(path + y, remain_data);
			int size_of_file = remain_data;
			std::ifstream fd;
			fd.open(path + y, std::ios::binary);

			// std::cout << "\n\nOpen status" << fd << std::endl;
			// std::cout << "File sent status" << file_sent_status[x.first] << std::endl;
			// std::cout << "Successfully sent counter" << successfully_sent_counter << std::endl;
			// std::cout << path + y << std::endl;
			// std::cout << "Remaining data- " << remain_data << std::endl;
			// std::cout << "Client socket number- " << cnt << std::endl;

			// while (true)
			// {
			while (remain_data > 0)
			{
				bzero(buffer, BUFSIZ);
				fd.seekg(size_of_file - remain_data);
				fd.read(buffer, BUFSIZ);
				// sent_bytes = sendfile(client_socket[cnt], fd, (off_t *)&offset, remain_data);
				sent_bytes = send(client_socket[cnt], buffer, BUFSIZ, 0);
				if (sent_bytes < 0)
				{
					// std::cout << "Client could not send" << std::endl;
					break;
				}
				else if (sent_bytes == 0)
				{
					// std::cout << "Zero data sent on " << client_socket[cnt] << std::endl;
					break;
				}
				remain_data -= sent_bytes;
				// std::cout << "Sent " << sent_bytes << " bytes from file's data, offset is now : " << offset << " and remaining data = " << remain_data << "\n";
			}
			auto start = std::chrono::system_clock::now(); // timer start
			while (true)
			{
				auto now = std::chrono::system_clock::now(); // timer end
				auto diff = now - start;
				double time = diff.count();
				if (time > 100000000)
				{
					break;
				}
			}
			// }
			if (remain_data <= 0)
			{
				// assert(fd.gcount() >= length_file);
				int ack = -1;
				while (ack <= 0)
				{
					ack = recv(client_socket[cnt], buffer, 1000, 0);
				}
				std::string file_acked = buffer;
				assert(strstr(y.c_str(), file_acked.c_str()) || strstr(file_acked.c_str(), y.c_str()));
				file_sent_status[x.first]++;
				// std::cout << "Sent a FILE. Also Acked. " << y << std::endl;
			}
			if (file_sent_status[x.first] == x.second.size())
			{
				successfully_sent_counter++;
				// std::cout << "Sent all to one Server " << successfully_sent_counter << std::endl;
			}
			++cnt;
			fd.close();
		}
	}

	std::map<std::string, std::pair<int, int>> fm;
	std::vector<std::vector<int>> svt;
	std::set<std::string> files_still_to_download;
	bool flag = false;
	for (auto x : files_to_download)
	{

		for (auto y : server_map)
		{
			for (auto z : y.second)
			{
				if (x.compare(z) == 0)
				{
					flag = true;
				}
			}
		}
		if (!flag)
		{
			files_still_to_download.insert(x);
		}
	}
	// for (int i = 0; i < num_neighbour; i++)
	// {
	// 	read(client_socket[i], buffer, BUFSIZ);
	// 	std::string buf = buffer;
	// 	// std::cout << buf;
	// 	std::vector<std::string> pieces;
	// 	tokenize(buf, '\n', pieces);
	// 	// std::cout << ID << " " << "yo\n";
	// 	bzero(buffer, BUFSIZ);

	// 	// so now I will dump all the info to the client side and then process it all here itself.
	// 	// Will also be sensible as I will all the info about both 1 depth and 2 depth neighbour.

	// 	// need to process the string to extract the info about files and stuff
	// 	extract_info(pieces, files_still_to_download, fm, svt);
	// 	close(client_socket[i]);
	// }
	// std::sort(svt.begin(), svt.end(), [](const std::vector<int> &a, const std::vector<int> &b)
	// 		  { return a[0] < b[0]; });

	// for (auto x : svt)
	// {

	// 	std::cout << "Connected to " << x[0] << " with unique-ID " << x[1] << " on port " << x[2] << std::endl;
	// }

	// // fm contains the ID,PORT for every file.
	// // also a new file_map for sizes.
	// std::map<int, std::set<std::string>> socket_2;
	// std::map<int, int> port_2_id;
	// for (auto x : fm)
	// {
	// 	port_2_id[x.second.second] = x.second.first;
	// 	socket_2[x.second.second].insert(x.first);
	// }
	// auto socket_22 = socket_2;
	// while (!socket_22.empty())
	// {
	// 	socket_2 = socket_22;
	// 	for (auto x : socket_2)
	// 	{
	// 		if (x.second.empty())
	// 		{
	// 			socket_22.erase(x.first);
	// 			continue;
	// 		}
	// 		int valread;
	// 		msg = *(x.second.begin());
	// 		std::cout<<msg<<std::endl;
	// 		const char *msg2 = msg.c_str();
	// 		int sd = socket(AF_INET, SOCK_STREAM, 0);

	// 		neighbour_address[0].sin_family = AF_INET;
	// 		neighbour_address[0].sin_port = htons(x.first);
	// 		neighbour_address[0].sin_addr.s_addr = inet_addr("127.0.0.1");
	// 		memset(&(neighbour_address[0].sin_zero), '\0', 8);

	// 		int status = -1;
	// 		while (status == -1)
	// 		{
	// 			status = connect(sd, (struct sockaddr *)&neighbour_address[0], sizeof(neighbour_address[0]));
	// 			if (status > -1)
	// 			{
	// 				send(sd, msg2, strlen(msg2), 0);

	// 				ssize_t len;
	// 				off_t remain_data;
	// 				std::ofstream received_file;
	// 				received_file.open(path + "Downloaded/" + msg, std::ios::binary);

	// 				// FILE *received_file = fopen((path + "Downloaded/" + file_name).c_str(), "w");
	// 				bzero(buffer, BUFSIZ);

	// 				remain_data = file_size_dist2[msg];
	// 				// std::cout << "Size of file " << remain_data << std::endl;
	// 				while (remain_data > 0)
	// 				{
	// 					// std::cout << "rec" << std::endl;
	// 					len = recv(sd, buffer, BUFSIZ, 0);
	// 					if (len < 0)
	// 					{
	// 						// std::string mkdir = "mkdir -p " + path + "Error" + std::to_string(ID) + file_name;
	// 						// system(mkdir.c_str());
	// 						received_file.close();
	// 						// std::cout << "Error while receiving " << file_name << std::endl;
	// 						break;
	// 					}
	// 					if (len == 0)
	// 					{
	// 						// std::string mkdir = "mkdir -p " + path + "AHHHHHHHHH" + std::to_string(ID) + file_name;
	// 						// system(mkdir.c_str());
	// 						received_file.close();
	// 						// std::cout << "Error while receiving " << file_name << std::endl;
	// 						break;
	// 					}
	// 					if (remain_data <= len)
	// 					{
	// 						int ack = -1;
	// 						while (ack <= 0)
	// 						{
	// 							ack = send(sd, msg2, strlen(msg2), 0);
	// 						}
	// 						// char *buffer_new = new char[remain_data];
	// 						// bzero(buffer_new, remain_data);
	// 						// strncpy(buffer_new, buffer, remain_data);
	// 						// received_file.write(buffer_new, remain_data);
	// 						received_file.write(buffer, remain_data);
	// 						x.second.erase(x.second.find(msg));
	// 						// received_file << std::string(buffer_new).substr(0,remain_data);
	// 						std::cout << "NEW Received a file. Also acked" << std::endl << std::endl;
	// 						remain_data = 0;
	// 					}
	// 					else
	// 					{
	// 						remain_data -= len;
	// 						// till here buffer is raw.
	// 						received_file.write(buffer, len);
	// 						// received_file << std::string(buffer).substr(0,BUFSIZ);
	// 					}
	// 					bzero(buffer, BUFSIZ);
	// 					// std::cout << "Received " << len << " bytes and remaining -> " << remain_data << " bytes" << std::endl;
	// 				}

	// 				received_file.close();

	// 				close(sd);
	// 			}
	// 		}
	// 	}
	// }

	// auto vecst = set_diff(files_to_download, get_keys(fm));
	// for (auto x : vecst)
	// {
	// 	fm[x] = std::make_pair(0, 0);
	// }
	// for (auto x : fm)
	// {
	// 	int depth = x.second.first;
	// 	int client_sno = x.second.second;
	// 	if (client_sno != 0)
	// 	{
	// 		// actually found.
	// 		std::cout << "Found " << x.first << " at " << client_sno << " with MD5 "+get_hash(path+"Downloaded/"+x.first)+" at depth " << depth << std::endl;
	// 	}
	// 	else
	// 	{
	// 		std::cout << "Found " << x.first << " at " << client_sno << " with MD5 0 at depth " << depth << std::endl;
	// 	}
	// }
	// std::cout << S_NO << "    DONEDONE" << std::endl;
}

void server(int PORT, std::vector<std::string> files_to_download, int num_neighbours,
			std::vector<int> &neighbour_client_number, int ID, std::string path, int S_NO, std::vector<int> neighbour_ports)
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
	if (listen(master_socket, 30) < 0)
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
	std::map<int, std::string> files_with_client;

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
					// std::cout << "Socket number of no. " << w2 << " with ID " << w << " is " << sd << std::endl;
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
					files_with_client[w] = get_file_names_as_string(str2);
					m[atoi(word2.c_str())] = prnt;
				}
			}
		}
	}

	for (auto x : file_map_no)
	{
		file_client_map[x.second].push_back(x.first);
	}
	std::string client_files_info, client_data[num_neighbours];
	int index = 0;
	for (auto i = files_with_client.begin(); i != files_with_client.end(); i++)
	{
		client_files_info += "2:" + std::to_string(i->first) + "|" + std::to_string(neighbour_ports[index]) + "\n" + i->second;
		index++;
		// 2:uniqueid|port\n data
	}
	client_files_info += (std::to_string(ID) + "," + std::to_string(S_NO) + "," + std::to_string(PORT));

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
		// else
		// {
		// 	// std::cout << "FILE CLIENT MAP BEGIN for " << current_socket.first << std::endl;
		// 	// for (auto files : file_client_map[current_socket.first])
		// 	// {
		// 	// 	std::cout << files << std::endl;
		// 	// }
		// 	// std::cout << "FILE CLIENT MAP END for " << current_socket.first << std::endl;
		// }
		file_rec_status[current_socket.first] = 0;
	}

	max_sd = 0;
	auto client_socket_ordered_copy = client_socket_ordered;
	for (auto current_socket : client_socket_ordered)
	{
		if (file_rec_status[current_socket.first] == file_client_map[current_socket.first].size())
		{
			client_socket_ordered_copy.erase(current_socket.first);
		}
	}
	client_socket_ordered = client_socket_ordered_copy;
	// assert(sockets_alive.size() == num_neighbours);

	while (num_received_success < num_neighbours)
	{
		// std::cout << "I am alive" << std::endl;
		// std::cout << "I am seeing some activity" << std::endl;
		client_socket_ordered = client_socket_ordered_copy;
		for (auto current_socket : client_socket_ordered)
		{
			// std::cout << "Message from " << current_socket.first << " on socket " << current_socket.second << std::endl;

			std::string file_name = file_client_map[current_socket.first][file_rec_status[current_socket.first]];
			// receiving data.
			/// maintain a 2d array which keeps track of what is received. In order receiving.
			// Variable which maintains the file number till where we have received, for each client.
			// int array of num_neighbour

			// std::cout << "ACTIVITY ON- " << current_socket.first << " with file" << file_name << std::endl;
			// std::cout << "Current Status " << file_rec_status[current_socket.first] << std::endl;
			// std::cout << "Total Required " << file_client_map[current_socket.first].size() << std::endl;
			ssize_t len;
			int sent_bytes = 0;
			off_t remain_data;
			std::ofstream received_file;
			received_file.open(path + "Downloaded/" + file_name, std::ios::binary);

			// FILE *received_file = fopen((path + "Downloaded/" + file_name).c_str(), "w");
			bzero(buffer, BUFSIZ);

			remain_data = file_sz_map[file_name];
			// std::cout << "Size of file " << remain_data << std::endl;
			while (remain_data > 0)
			{
				// std::cout << "rec" << std::endl;
				len = recv(current_socket.second, buffer, BUFSIZ, 0);
				if (len < 0)
				{
					// std::string mkdir = "mkdir -p " + path + "Error" + std::to_string(ID) + file_name;
					// system(mkdir.c_str());
					received_file.close();
					// std::cout << "Error while receiving " << file_name << std::endl;
					break;
				}
				if (len == 0)
				{
					// std::string mkdir = "mkdir -p " + path + "AHHHHHHHHH" + std::to_string(ID) + file_name;
					// system(mkdir.c_str());
					received_file.close();
					// std::cout << "Error while receiving " << file_name << std::endl;
					break;
				}
				if (remain_data <= len)
				{
					int ack = -1;
					while (ack <= 0)
					{
						ack = send(current_socket.second, file_name.c_str(), strlen(file_name.c_str()), 0);
					}
					// char *buffer_new = new char[remain_data];
					// bzero(buffer_new, remain_data);
					// strncpy(buffer_new, buffer, remain_data);
					// received_file.write(buffer_new, remain_data);
					received_file.write(buffer, remain_data);

					// received_file << std::string(buffer_new).substr(0,remain_data);
					file_rec_status[current_socket.first]++;
					// std::cout << "Received a file. Also acked" << std::endl << std::endl;
					remain_data = 0;
				}
				else
				{
					remain_data -= len;
					// till here buffer is raw.
					received_file.write(buffer, len);
					// received_file << std::string(buffer).substr(0,BUFSIZ);
				}
				bzero(buffer, BUFSIZ);
				// std::cout << "Received " << len << " bytes and remaining -> " << remain_data << " bytes" << std::endl;
			}

			if (file_rec_status[current_socket.first] == file_client_map[current_socket.first].size())
			{
				client_socket_ordered_copy.erase(current_socket.first);
				num_received_success++;
				// printf("One server done %d", current_socket.first);
				// std::cout << std::endl;
			}
			received_file.close();
		}
	}

	bool counter[num_neighbours] = {};
	int count = 0;

	while (count < num_neighbours)
	{
		for (int i = 0; i < num_neighbours; i++)
		{
			if (counter[i])
			{
				continue;
			}
			// send the data to
			// client_socket[i] and, basically, which file is where.
			int val = -1;
			auto start = std::chrono::system_clock::now(); // timer begin
			auto now = start;
			std::chrono::duration<double> diff = now - start;
			double time;
			while (val == -1)
			{
				val = send(client_socket[i], client_files_info.c_str(), strlen(client_files_info.c_str()), 0);
				now = std::chrono::system_clock::now(); // timer end
				diff = now - start;
				time = diff.count();
				if (time > 0.1)
				{
					break;
				}
			}
			if (val != -1)
			{
				counter[i] = true;
				count++;
				close(client_socket[i]);
			}
		}
	}

	// for (int i = 0; i < 30; i++)
	// {
	// 	client_socket[i] = 0;
	// }
	// struct timeval time_out;
	// time_out.tv_sec = 10;
	// while (true)
	// {
	// 	FD_ZERO(&readfds);
	// 	FD_SET(master_socket, &readfds);
	// 	max_sd = master_socket;
	// 	for (int i = 0; i < max_clients; i++)
	// 	{
	// 		sd = client_socket[i];
	// 		if (sd > 0)
	// 			FD_SET(sd, &readfds);
	// 		if (sd > max_sd)
	// 			max_sd = sd;
	// 	}
	// 	activity = select(max_sd + 1, &readfds, NULL, NULL, &time_out);

	// 	if ((activity < 0) && (errno != EINTR))
	// 	{
	// 		break;
	// 	}
	// 	if (FD_ISSET(master_socket, &readfds))
	// 	{
	// 		if ((new_socket = accept(master_socket,
	// 								 (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
	// 		{
	// 			perror("accept");
	// 			exit(EXIT_FAILURE);
	// 		}

	// 		for (int i = 0; i < max_clients; i++)
	// 		{
	// 			if (client_socket[i] == 0)
	// 			{
	// 				client_socket[i] = new_socket;
	// 				break;
	// 			}
	// 		}
	// 	}

	// 	for (int i = 0; i < max_clients; i++)
	// 	{
	// 		sd = client_socket[i];

	// 		if (FD_ISSET(sd, &readfds))
	// 		{
	// 			if ((valread = read(sd, buffer, BUFSIZ)) == 0)
	// 			{
	// 				getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
	// 				close(sd);
	// 				client_socket[i] = 0;
	// 			}

	// 			else
	// 			{
	// 				buffer[valread] = '\0';
	// 				std::string str = buffer;
	// 				std::string str2 = buffer;
	// 				ssize_t len;
	// 				int offset = 0, sent_bytes = 0;
	// 				off_t remain_data;
	// 				///check if file actually exists here.
	// 				get_file_len(path + str, remain_data);
	// 				int size_of_file = remain_data;
	// 				std::ifstream fd;
	// 				fd.open(path + str, std::ios::binary);

	// 				while (remain_data > 0)
	// 				{
	// 					bzero(buffer, BUFSIZ);
	// 					fd.seekg(size_of_file - remain_data);
	// 					fd.read(buffer, BUFSIZ);
	// 					// sent_bytes = sendfile(client_socket[cnt], fd, (off_t *)&offset, remain_data);
	// 					sent_bytes = send(sd, buffer, BUFSIZ, 0);
	// 					if (sent_bytes < 0)
	// 					{
	// 						// std::cout << "Client could not send" << std::endl;
	// 						break;
	// 					}
	// 					else if (sent_bytes == 0)
	// 					{
	// 						// std::cout << "Zero data sent on " << client_socket[cnt] << std::endl;
	// 						break;
	// 					}
	// 					remain_data -= sent_bytes;
	// 					// std::cout << "Sent " << sent_bytes << " bytes from file's data, offset is now : " << offset << " and remaining data = " << remain_data << "\n";
	// 				}
	// 				auto start = std::chrono::system_clock::now(); // timer start
	// 				while (true)
	// 				{
	// 					auto now = std::chrono::system_clock::now(); // timer end
	// 					auto diff = now - start;
	// 					double time = diff.count();
	// 					if (time > 100000000)
	// 					{
	// 						break;
	// 					}
	// 				}
	// 				// }
	// 				if (remain_data <= 0)
	// 				{
	// 					// assert(fd.gcount() >= length_file);
	// 					int ack = -1;
	// 					while (ack <= 0)
	// 					{
	// 						ack = recv(sd, buffer, 1000, 0);
	// 					}
	// 					std::string file_acked = buffer;
	// 					assert(strstr(str.c_str(), file_acked.c_str()) || strstr(file_acked.c_str(), str.c_str()));
	// 					// std::cout << "Sent a FILE. Also Acked. " << y << std::endl;
	// 				}
	// 				fd.close();
	// 			}
	// 		}
	// 	}
	// }
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

	std::thread t1(server, PORT, std::ref(files_to_download), num_neighbour,std::ref(neighbour_client_number), ID, std::ref(path), S_NO, std::ref(neighbour_client_port));

	std::thread t2(client, S_NO, num_neighbour, std::ref(neighbour_client_port),
				   std::ref(neighbour_client_number), PORT, ID, std::ref(path),
				   std::ref(files_to_download));

	t1.join();
	t2.join();

	return 0;
}
