#include <iostream>
#include <sys/socket.h> //sockets
#include <sys/un.h> // Unix domain socket
#include <unistd.h> //unlink
#include <cstring>
#include <string>
#include <ctime>
using namespace std;

int main (int argc, char* argv[]){
	int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	 //AF_UNIX means local socket, communicating with system files
	 //SOCK_STREAM means stable connection, 0:default protocol

	// predefined struct sockaddr_un : 1. type of socket - 2.path
	struct sockaddr_un server_addr;
	// socket type
	server_addr.sun_family = AF_UNIX;
	//path
	strcpy(server_addr.sun_path, "my_socket");

	//before binding we should delete the existing file or we get error
	unlink("my_socket");

	// bind is common for every socket, we need to cast to the structure of linux
	if(bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1){
		cerr << "Bind failed\n";
	}

	if( listen(server_fd, 5) == -1){
		cerr <<"Listen failed.\n";
		return 1;
	}
	cout << "Server is listening on my_socket...\n";
	while(true){
		//pauses app untill a client connects
		//nullptr because address of clinet isn't needed yet
		int client_fd = accept(server_fd, nullptr, nullptr);
		if(client_fd == -1){
			cerr << "Client accept failed\n";
			continue;
		}
		cout << "New Clinet connected.\n";
		
		char buffer[1024] = {}; //holder for input
		recv(client_fd, buffer, sizeof(buffer), 0);
		string request(buffer);
		cout << "Client asked: " << request << endl;

		if(request == "time"){
			time_t now = time(0);
			string time_str = ctime(&now);

			send(client_fd, time_str.c_str(), time_str.length(),0);
		}else if(request.substr(0,5) == "echo "){
			string echowhat = request.substr(5);
			send(client_fd, echowhat.c_str(), echowhat.length(), 0);
		}else{
			string error_msg = "Invalid command!\n";
			send(client_fd, error_msg.c_str(), error_msg.length(),0);
		}
		close(client_fd);
	}
	return 0;
}
