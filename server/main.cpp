#include "network.h"

#include <iostream>
#include <cstdlib>
//#include <signal.h>

using namespace std;

int main(int argc, char *argv[]) {
	network server;

run:
	if(server.init() == EXIT_FAILURE) { // init the connexion and wait the client
		cout << "Sorry, an error has occurred during the initialization. End." << endl;
		return EXIT_FAILURE;
	}
	
	if(server.run() == EXIT_FAILURE) { // run the threads
		cout << "Sorry, an error has occurred. End." << endl;
		return EXIT_FAILURE;
	}
	else {
		char answer = 'n';
		cout << "Do you want to wait a new client ? (Y/n) ";
		cin >> answer;
		if(answer == 'Y')
			goto run;
	}
	
	return EXIT_SUCCESS;
}
