#ifndef _SERVER
#define _SERVER 1

#include <string>
using namespace std;

class Server {
	
	// Must be format (username password);
	void addManager(string);

	void setPort(int);

	//Main control loop
	//Returns when the server should exit
	void start(void);
	
	void stop(void);
	
};
#endif
