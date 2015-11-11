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


inline void init_error() {
	cout << "Sorry, an error has occurred during the initialization. End." << endl;
	exit(EXIT_FAILURE);
}
network::network() {
	SOCKADDR_IN sin;
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1) {
		perror("socket");
		init_error();
	}
	
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);

	if(bind(sock, (SOCKADDR*)&sin, sizeof(sin)) == -1) {
		perror("bind");
		init_error();
	}
	
	if(listen(sock, 5) == -1) {
		perror("listen");
		init_error();
	}
}

int network::init() {

waiting_client:
	cout << "Waiting for a client connexion..." << endl;
	
	SOCKADDR_IN csin;
	socklen_t crecsize = sizeof(csin);
	csock = accept(sock, (SOCKADDR*)&csin, &crecsize);
	cout << "The client is now connected from " << inet_ntoa(csin.sin_addr) << " !" << endl;
	
	
	// ENCRYPTION INITIALIZATION
	cout << "Generating RSA key pair... " << endl;
	srand(time(NULL)); // Initialize random for the RSA prime generator.
	KeyPair key_pair(RSA::GenerateKeyPair(20));
	cout << "RSA public key : " << key_pair.GetPublicKey().GetModulus() << " " << key_pair.GetPublicKey().GetExponent() << endl << "RSA private key : " << key_pair.GetPrivateKey().GetModulus() << " " << key_pair.GetPrivateKey().GetExponent() << endl;
	
	cout << "Sending RSA public key to client..." << endl;
	// I can't send the key object himself, because BigInt manipulates pointers (for arrays). So I just send two strings.
	char public_key[100] = "";
	strcpy(public_key, const_cast<char*>(key_pair.GetPublicKey().GetModulus().ToString().c_str()));
	strcat(public_key, "-"); strcat(public_key, const_cast<char*>(key_pair.GetPublicKey().GetExponent().ToString().c_str()));
	send(csock, public_key, 100, 0);
	
	
	string password;
	cout << "Password ? ";
	cin >> password;
	
	cout << "Waiting for the Blowfish key..." << endl;
	char encrypted_blowfish_key[100];
	recv(csock, encrypted_blowfish_key, 500, 0);
	cout << "Encrypted blowfish key : " << encrypted_blowfish_key << endl << "Decrypting Blowfish key with RSA..." << endl;
	string blowfish_key = RSA::Decrypt(encrypted_blowfish_key, key_pair.GetPrivateKey());
	if(blowfish_key == password) {
		blowfish.init_with_key(blowfish_key);
		cout << "Ready to chat !" << endl;
	}
	else {
		cout << "PASSWORD ERROR" << endl;
		close(csock);
		cout << "CLIENT KICKED" << endl;
		goto waiting_client;
	}
	
	
	return EXIT_SUCCESS;
}


int network::run_send() { // Note : the send thread is placed in the actual thread. It closes automatically the program if it sends "END".
	string msg = "";
	do{
		getline(cin, msg);
		if(msg == "")
			continue;
		string enc_msg = blowfish.encrypt(msg);
		if(enc_msg.size() < 1000)
			send(csock, enc_msg.c_str(), enc_msg.size(), 0);
		else
			cout << "Sorry, this message is too long to be send. The max size of a message is 1 000 bytes (encrypted)." << endl;
	}while(msg != "END");
	
	
	close(csock);
	cout << "Client kicked." << endl;	
    	
    	return EXIT_SUCCESS;
}

void *recv_thread(void *arg){
	SOCKET csock = (*reinterpret_cast<network*>(arg)).get_csock();
	SOCKET sock = (*reinterpret_cast<network*>(arg)).get_sock();
	Blowfish blowfish = (*reinterpret_cast<network*>(arg)).get_blowfish();
	
	string msg = "";
	do{
		char str[1000] = "";
		recv(csock, str, 1000, 0);
		msg = str;
		msg = blowfish.decrypt(msg);
		if(msg == "") // If we receive an empty message, it's probably because we have kicked the client, or because he is gone. So we must exit this thread.
			pthread_exit(NULL);
		cout << "> " << msg << endl;
	}while(msg != "END");
	
	shutdown(sock, 2);
	close(sock);
        cout << "The client terminated the connexion. Its socket has been automatically closed." << endl;
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

network::~network() {
	shutdown(sock, 2);
	close(csock);
        close(sock);
}
