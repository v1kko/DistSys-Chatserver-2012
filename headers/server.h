#ifndef _SERVER
#define _SERVER 1

#include <string>
#include <manager.h>
#include <connection.h>
#include <database.h>

using namespace std;

class Server {
private:
	Connection * connection;
	Manager * manager;
	Database * database;
	string ident, parentname;
	unsigned long csip, parentip, parentport, csref;
	unsigned short port;

	void incomingMessage(Message);
	void monitor(void);
	
public:
	//port, csip, ident
	Server(unsigned short, unsigned long, string);
	// Must be format (username password);
	void addManager(string);

	//Main control loop
	//Returns when the server should exit
	void start(void);
	
	void stop(void);	
};
#endif
