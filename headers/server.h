#ifndef _SERVER
#define _SERVER 1

#include <string>
#include <manager.h>
#include <connection.h>
#include <database.h>

#define CSTIMEOUT 3
#define SELECT 10

using namespace std;

class Server {
private:
	Connection * connection;
	Manager * manager;
	Database * database;
	string ident, parentname;
	unsigned long noparenttimeout, cstimeout, csip, parentfirst, parentip, parentport, csref;
	unsigned short csport;

	void incomingMessage(Message);
	void monitor(void);
	
public:
	//port, csip, ident
	Server(unsigned short, string, unsigned short, string, unsigned int);
	// Must be format (username password);
	void addManager(string);

	//Main control loop
	//Returns when the server should exit
	void start(void);
	
	void stop(void);	
};
#endif
