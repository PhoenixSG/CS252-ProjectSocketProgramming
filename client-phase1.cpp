#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>



void convert_file_into_words (std::string file);

int main(int argc, char** argv){
	std::string file = argv[1];
	std::string path = argv[2];	
	std::string commd = "cd " + path + ";" + "ls -p | grep -v /";
	system(commd.c_str());
	//convert_file_into_words(file);
/*
	int socket_desc , new_socket;
	struct sockaddr_in server , client;
	
	
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
		puts("Could not create socket");
	
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(MYPORT);
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
		puts("bind failed");
	
	puts("bind done");
	
	
	listen(socket_desc , 3);
	
	
	
		//puts("Waiting for incoming connections...");
		
		c = sizeof(struct sockaddr_in);

		

		new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);

		

		if (new_socket < 0)
			perror("accept failed");
		
		puts("Connection accepted");
		
		while(1){
			char buf[20], message[20];
			recv(new_socket, buf, 20, 0);
			snprintf(message, 20, "%d", parse(remove_white_spaces(buf)));
			//itoa(parse(remove_white_spaces(buf)), message, 10);
			send(new_socket, message, 20, 0);

		}
		

		close(new_socket);
		close(socket_desc);
*/
	
	return 0;

}
/*
void convert_file_into_words(std::string file){
   
   std::string line, word;
   int counter = 0;

   ifstream in(file);
   while ( getline( in, line ) ){
      counter++;
      std::cout << "\nLine " << counter << ":\n";
      std::stringstream ss( line );
      while ( ss >> word ) cout << word << '\n';
   }

   in.close();
}
*/

