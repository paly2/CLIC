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
	SOCKADDR_IN csin;
	socklen_t crecsize = sizeof(csin);
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1) {
		perror("socket");
		return EXIT_FAILURE;
	}
	
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);

	if(bind(sock, (SOCKADDR*)&sin, sizeof(sin)) == -1) {
		perror("bind");
		return EXIT_FAILURE;
	}
	
	if(listen(sock, 5) == -1) {
		perror("listen");
		return EXIT_FAILURE;
	}
	
	cout << "Wait the client connexion..." << endl;
	csock = accept(sock, (SOCKADDR*)&csin, &crecsize);
	cout << "The client is now connected from " << inet_ntoa(csin.sin_addr) << ":" << ntohs(csin.sin_port) << " !" << endl;
	
	return EXIT_SUCCESS;
}

int network::run_send() { // Note : the send thread is placed in the actual thread. It closes automatically the program if it sends "END".
	string str = "";
	do{
		getline(cin, str);
		if(str.length() < 1000) 
			send(csock, str.c_str(), 1000, 0);
		else
			cout << "Sorry, this message is too long to be send. The max lenght of a message is 1 000 characters." << endl;
	}while(str != "END");
	
	end();
	exit(EXIT_SUCCESS);
    	
    	return EXIT_SUCCESS;
}

void *recv_thread(void *arg){
	SOCKET csock = (*reinterpret_cast<network*>(arg)).get_csock();
	SOCKET sock = (*reinterpret_cast<network*>(arg)).get_sock();
	char str[1000] = "";
        do{
            recv(csock, str, 1000, 0);
            if(strcmp(str, "") != 0)
            	cout << "> " << str << endl;
        }while(strcmp(str, "END") != 0);
        
        shutdown(sock, 2);
        close(sock);
        close(csock);
        cout << "The client has terminated the connexion. Sockets have been automatically closed. Press Ctrl+C to close this server program." << endl;
        pthread_exit(NULL);
        
}
int network::run_recv() { // Note : the reception thread is placed in another thread.
	pthread_t recv_thread_t;
	if(pthread_create(&recv_thread_t, NULL, recv_thread, reinterpret_cast<void*>(this)) == -1) {
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
	shutdown(sock, 2);
	close(csock);
        close(sock);
}

SOCKET network::get_csock() {
	return csock;
}

SOCKET network::get_sock() {
	return sock;
}

network::~network() {
	end();
}
