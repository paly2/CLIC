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

#include <ctime>
#include "../rsa/RSA.h"
#include "../Blowfish/blowfish.h"

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
	if(connect(sock, (SOCKADDR*)&sin, sizeof(sin)) == -1) {
		cout << "Sorry, the connexion failed :" << endl;
		perror("connect");
		return EXIT_FAILURE;
	}
	cout << "The connexion succed !" << endl;
	
	
	// ENCRYPTION INITIALIZATION
	cout << "Waiting for receiving RSA public key..." << endl;
	char rsa_key_recv[100] = "";
	recv(sock, rsa_key_recv, 100, 0);
	const string modulus(strtok(rsa_key_recv, "-"));
	const string exponent(strtok(NULL, "-"));
	Key rsa_key(modulus, exponent);
	cout << "RSA public key : " << rsa_key.GetModulus() << " " << rsa_key.GetExponent() << endl;
	
	
	string blowfish_key;
	cout << "Password ? ";
	cin >> blowfish_key;
	blowfish.init_with_key(blowfish_key);
	
	cout << endl << "Encrypting Blowfish key with RSA..." << endl;
	string encrypted_blowfish_key = RSA::Encrypt(blowfish_key, rsa_key);
	cout << "Encrypted Blowfish key is  : " << encrypted_blowfish_key << endl << "Its size is : " << encrypted_blowfish_key.size() << endl << "Send encrypted blowfish key to server..." << endl;
	send(sock, encrypted_blowfish_key.c_str(), encrypted_blowfish_key.length(), 0);
	cout << "Ready to chat !" << endl;
	
	
	return EXIT_SUCCESS;
}

int network::run_send() { // Note : the send thread is placed in the actual thread. It closes automatically the program if it sends "END".
	string msg = "";
	do {
		getline(cin, msg);
		if(msg == "")
			continue;
		string enc_msg = blowfish.encrypt(msg);
		if(enc_msg.size() < 1000)
			send(sock, enc_msg.c_str(), enc_msg.size(), 0);
		else
			cout << "Sorry, this message is too long to be send. The max length of a message is 1 000 bytes (encrypted)." << endl;
	} while(msg != "END");
	
    	close(sock);
    	exit(EXIT_SUCCESS);
    	
    	return EXIT_SUCCESS;
}

void *recv_thread(void *arg){
	SOCKET sock = (*reinterpret_cast<network*>(arg)).get_sock();
	Blowfish blowfish = (*reinterpret_cast<network*>(arg)).get_blowfish();
	string msg = "";
	do{
		char str[1000] = "";
		recv(sock, str, 1000, 0);
		msg = blowfish.decrypt(str);
		if(msg == "") {
			cout << "Error : empty message received. Is this client kicked ?" << endl;
			close(sock);
			exit(EXIT_FAILURE);
		}
		cout << "> " << msg << endl;
	}while(msg != "END");
	
	close(sock);
	cout << "The server has terminated the connexion. Sockets have been automatically closed. Press Ctrl+C to close this client program." << endl;
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
        close(sock);
}

network::~network() {
	end();
}
