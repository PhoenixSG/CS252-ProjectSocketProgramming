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
	std::string commd = "cd " + path + ";" + "ls -1vp | grep -v /";
	system(commd.c_str());
}

std::vector<std::string> get_file_names(std::string msg)
{

	auto otp = msg.substr(msg.find(";"));
	otp = &otp[1];
	std::vector<std::string> retval;
	tokenize(otp, '\n', retval);
	return retval;
}

std::string get_file_names_as_string(std::string msg)
{
	auto otp = msg.substr(msg.find(";"));
	otp = &otp[1];
	return otp;
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

std::vector<std::string> intersection(std::vector<std::string> &v1,
									  std::vector<std::string> &v2)
{
	std::vector<std::string> v3;

	std::sort(v1.begin(), v1.end());
	std::sort(v2.begin(), v2.end());

	std::set_intersection(v1.begin(), v1.end(),
						  v2.begin(), v2.end(),
						  back_inserter(v3));
	return v3;
}

std::string generate_message(std::vector<std::string> files, std::string ID)
{

	std::string retval = "";
	for (auto x : files)
		retval += ("Found " + x + " at " + ID + " with MD5 0 at depth 1\n");
	return retval;
}

bool contains_colon(std::string s)
{
	if (s.find(':') != std::string::npos)
		return true;
	else
		return false;
}

void display(std::pair<int, int> x)
{
	std::cout << "(" << x.first << " ," << x.second << ")\n";
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

void extract_info(std::vector<std::string> svs, 
std::vector<std::string> &files_to_download, 
std::map<std::string, std::pair<int, int>> &fm)
{

	std::map<std::pair<int, int>, std::vector<std::string>> m;
	std::vector<std::string> temp;
	// std::map<std::string, std::pair<int, int>> fm;
	int sz = svs.size();
	int i = 0;
	for (; i < sz; ++i)
	{
		std::string x = svs[i];
		if (x.empty())
			continue;
		if (contains_colon(x))
		{
			std::pair<int, int> p;
			p.first = int(x[0]) - '0'; // p.first is the depth
			p.second = std::stoi(x.substr(x.find(":") + 1));
			int j = i + 1;
			for (; (j < sz) && !contains_colon(svs[j]); ++j)
			{
				temp.push_back(svs[j]);
			}
			m[p] = intersection(temp, files_to_download);
			for (std::string file : m[p])
			{
				if (fm.find(file) == fm.end())
				{
					fm[file] = p;
				}
				else
				{
					//std::cout << "Entered            ";
					//display(fm[file]);
					fm[file] = minimum(p, fm[file]);
					//std::cout << "Exited            ";
					//display(fm[file]);
				}
			}
			i = j - 1;
			temp.clear();
		}
	}

	// for (auto x : fm)
	// {
	// 	int depth = x.second.first;
	// 	int client_sno = x.second.second;

	// 	std::cout << "Found " << x.first << " at " << client_sno << " with MD5 0 at depth " << depth
	// 			  << std::endl;
	// }

	// for(auto x : m){
	// 	int depth = x.first.first;
	// 	int client_sno = x.first.second;
	// 	for(auto y : x.second){
	// 		std::cout << "Found " << y << " at " << client_sno << " with MD5 0 at depth " << depth
	// 		<< std::endl;
	// 	}
	// }
}

void client(int S_NO, int num_neighbour, std::vector<int> &neighbour_client_port,
			std::vector<int> &neighbour_client_number, int PORT, int ID,
			std::string path, std::vector<std::string> &files_to_download)
{
	struct sockaddr_in neighbour_address[num_neighbour];
	int client_socket[num_neighbour];
	int status[num_neighbour];
	char buffer[1025];

	for (int i = 0; i < num_neighbour; i++)
	{

		int valread;
		std::string msg = "Connected to " + std::to_string(ID) + " with unique-ID " + std::to_string(S_NO) + " on port " + std::to_string(PORT) + ";" +
						  GetStdoutFromCommand("cd " + path + " ;ls -1vp | grep -v /");
		const char *msg2 = msg.c_str();
		char buffer[1024] = {0};
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
			}
		}
	}
	std::string server_data[num_neighbour];
	std::map<std::string, std::pair<int, int>> fm;
	for (int i = 0; i < num_neighbour; i++)
	{
		read(client_socket[i], buffer, 1025);
		std::string buf = buffer;
		// std::cout << buf;
		std::vector<std::string> pieces;
		tokenize(buf, '\n', pieces);
		server_data[i] = buffer;
		// std::cout << ID << " " << "yo\n";
		bzero(buffer, 1025);

		// so now I will dump all the info to the client side and then process it all here itself.
		// Will also be sensible as I will all the info about both 1 depth and 2 depth neighbour.

		// need to process the string to extract the info about files and stuff
		extract_info(pieces, files_to_download, fm);
	}
	for (auto x : fm)
	{
		int depth = x.second.first;
		int client_sno = x.second.second;

		std::cout << "Found " << x.first << " at " << client_sno << " with MD5 0 at depth " << depth
				  << std::endl;
	}
	// std::cout << ID << " above---------------------------------------------------\n";
}

void server(int PORT, std::vector<std::string> files_to_download, int num_neighbours, int ID, std::string &path, int S_NO)
{
	int opt = 1;
	int master_socket, addrlen, new_socket, client_socket[30], max_clients = 30, activity, i, valread, sd;
	int max_sd, num_connections = 0;
	struct sockaddr_in address;

	char buffer[1025];
	fd_set readfds;
	for (i = 0; i < max_clients; i++)
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
	if (listen(master_socket, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	addrlen = sizeof(address);
	std::map<std::string, int> file_map;
	std::map<int, std::string> m;
	std::map<int, std::string> files_with_client;
	while (num_connections < num_neighbours)
	{
		FD_ZERO(&readfds);
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;
		for (i = 0; i < max_clients; i++)
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

			for (i = 0; i < max_clients; i++)
			{
				if (client_socket[i] == 0)
				{
					client_socket[i] = new_socket;
					break;
				}
			}
		}

		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];

			if (FD_ISSET(sd, &readfds))
			{
				if ((valread = read(sd, buffer, 1024)) == 0)
				{
					getpeername(sd, (struct sockaddr *)&address,
								(socklen_t *)&addrlen);
					close(sd);
					client_socket[i] = 0;
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

					auto file_names = get_file_names(str2);
					// contains the files available at this client. Need to send this somehow to all my clients.
					auto files_available = intersection(file_names, files_to_download);
					for (auto x : files_available)
					{
						int w = std::stoi(word);
						if (file_map.find(x) == file_map.end())
						{
							file_map[x] = w;
						}
						else
						{
							file_map[x] = std::min(w, file_map[x]);
						}
					}

					auto prnt = process(str) + "\n";
					m[atoi(word2.c_str())] = prnt;
					files_with_client[std::stoi(word)] = get_file_names_as_string(str2);
					// word2 has the client number. We can store the files it has in a map.
				}
			}
		}
	}
	std::string client_files_info, client_data[num_neighbours];
	for (auto i = files_with_client.begin(); i != files_with_client.end(); i++)
	{
		client_files_info += "2:" + std::to_string(i->first) + "\n" + i->second;
	}
	std::string own_files = "1:" + std::to_string(S_NO) + "\n" +
							GetStdoutFromCommand("cd " + path + " ;ls -1vp | grep -v /");
	client_files_info += own_files;
	// By now, we know the files with each neighbour and also know the files wanted by us.
	// We somehow need to convey one of these to all our neighbours.
	// If in the same connection, we convey to the neighbour, which files we need, then the neighbour
	// can get search within its own neighbours and let us know if it has them.
	// This would involve sending the files needed from server to client and then the client would
	// use this info to ....nah, better to receive info about others than to send info about ourselves.
	// Because printing needs to be done at our end only
	// So each server will send the information about the files at its neighbours to its neighbours.
	// This way, the clients will know about their neighbours neighbours. Thus, done. Can print at the
	// clients themselves, but will see about that later.
	bool counter[num_neighbours] = {};
	int count = 0;
	/*
	for (auto x : m)
	{
		std::cout << x.second;
	}
	for (auto x : files_to_download)
	{
		if (file_map.find(x) == file_map.end())
		{
			file_map[x] = 0;
		}
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
	*/
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
			}
		}
	}
}

int main(int argc, char **argv)
{
	// display(minimum(std::make_pair(2,507), std::make_pair(1,8507)));
	////////////////////Processing

	// print_file_names(argv);

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

	std::thread t1(server, PORT, std::ref(files_to_download), num_neighbour, ID, std::ref(path), S_NO);

	std::thread t2(client, S_NO, num_neighbour, std::ref(neighbour_client_port),
				   std::ref(neighbour_client_number), PORT, ID, std::ref(path),
				   std::ref(files_to_download));

	t1.join();
	t2.join();

	return 0;
}
