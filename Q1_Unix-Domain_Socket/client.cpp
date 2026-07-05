#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <string>

using namespace std;

int main (int argc, char* argv[]){
	// having less than 2 input arguments, we should output userguide and exit.
	if(argc < 2){
		cerr << "Use like: " << argv[0] << " <time / echo smth>\n";
		return 1;
	}
	 //AF_UNIX means local socket, communicating with system files
	 //SOCK_STREAM means stable connection, 0:default protocol
	int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	// predefined struct sockaddr_un : 1. type of socket - 2.path
	struct sockaddr_un server_addr;
	// socket type
	server_addr.sun_family = AF_UNIX;
	//path
	strcpy(server_addr.sun_path, "my_socket");

	// the counterpart of accept in server
	if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
		cerr << "Connection failed!\n";
		close(client_fd);
		return 1;
	}
	// make char array to a string named request
	string request = "";
	for( int i = 1; i<argc ; i++){
		request += argv[i];
		if(i < argc - 1)
			request += " ";
	}
	//send request
	send(client_fd, request.c_str(), request.length(), 0);
	//holder for answer of the server
	char buffer[1024] = {0};
	recv(client_fd, buffer, sizeof(buffer),0);
	//server response printing
	cout << "--Server Response: \n" << buffer << endl;
	//closing client connection
	close(client_fd);
	return 0;

}
