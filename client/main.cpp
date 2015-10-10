#include "network.h"

#include <iostream>
#include <cstdlib>
//#include <signal.h>

using namespace std;

int main(int argc, char *argv[]) {
	network client;
	
	if(client.init() == EXIT_FAILURE) { // init the connexion and wait the client
		cout << "Sorry, an error has occurred during the initialization. End." << endl;
		return EXIT_FAILURE;
	}
	
	if(client.run() == EXIT_FAILURE) { // run the threads
		cout << "Sorry, an error has occurred. End." << endl;
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
