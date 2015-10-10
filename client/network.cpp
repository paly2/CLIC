#include "network.h"

#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>

using namespace std;


int network::init() {
	SOCKADDR_IN sin;
	string adress;
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	
	cout << "Server adress ? ";
	cin >> adress;
	sin.sin_addr.s_addr = inet_addr(adress.c_str());
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	
	cout << "Trying to connect to " << inet_ntoa(sin.sin_addr) << ":" << htons(sin.sin_port) << "..." << endl;
	if(connect(sock, (SOCKADDR*)&sin, sizeof(sin)) == -1){
		cout << "Sorry, the connexion failed :" << endl;
		perror("connect");
		return EXIT_FAILURE;
	}
	cout << "The connexion succed !" << endl;
	
	return EXIT_SUCCESS;
}

int network::run_send() { // Note : the send thread is placed in the actual thread. It closes automatically the program if it sends "END".
	string str = "";
	do{
		getline(cin, str);
		if(str.length() < 1000)
			send(sock, str.c_str(), 1000, 0);
		else
			cout << "Sorry, this message is too long to be send. The max lenght of a message is 1 000 characters." << endl;
	}while(str != "END");
	
    	close(sock);
    	exit(EXIT_SUCCESS);
    	
    	return EXIT_SUCCESS;
}

void *recv_thread(void *arg){
	SOCKET sock = (*reinterpret_cast<SOCKET*>(arg));
	char str[1000] = "";
	do{
		recv(sock, str, 1000, 0);
		cout << "> " << str << endl;
	}while(strcmp(str, "END") != 0);
	
	close(sock);
	cout << "The server has terminated the connexion. Sockets have been automatically closed. Press Ctrl+C to close this client program." << endl;
	pthread_exit(NULL);
}
int network::run_recv() { // Note : the reception thread is placed in another thread.
	pthread_t recv_thread_t;
	if(pthread_create(&recv_thread_t, NULL, recv_thread, reinterpret_cast<void*>(&sock)) == -1) {
		perror("pthread_create");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int network::run() {
	if(run_recv() == EXIT_FAILURE) // Run the reception thread, and continue only if success
		return 	EXIT_FAILURE;
	
	return run_send(); // Run the send thread, and at its end, return its return value
}

void network::end() {
        close(sock);
}

SOCKET network::get_sock() {
	return sock;
}

network::~network() {
	end();
}
