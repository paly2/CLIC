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
	~network();
	int init();
	int run();
	int run_send();
	int run_recv();
	void end();
	SOCKET get_sock() {
		return sock;
	}
	Blowfish get_blowfish() {
		return blowfish;
	}
	
	private:
	SOCKET sock;
	Blowfish blowfish;
};

#endif
