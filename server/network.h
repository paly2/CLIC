#ifndef NETWORK_DEF
#define NETWORK_DEF

#include <netinet/in.h>
#include "../Blowfish/blowfish.h"

#define PORT 23
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

class network
{
	public:
	network();
	~network();
	int init();
	int run();
	int run_send();
	int run_recv();
	SOCKET get_sock() {
		return sock;
	}
	SOCKET get_csock() {
		return csock;
	}
	Blowfish get_blowfish() {
		return blowfish;
	}
	
	private:
	SOCKET sock;
	SOCKET csock;
	Blowfish blowfish;
};

#endif
