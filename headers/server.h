#ifndef _SERVER
#define _SERVER 1

#include <string>
#include <manager.h>
#include <connection.h>
#include <database.h>

#define CSTIMEOUT 4
#define SELECT 9

using namespace std;

class Server {
private:
	Connection * connection;
	Manager * manager;
	Database * database;
	string ident, parentname;
	unsigned long noparenttimeout, cstimeout, csip, parentfirst, parentip, parentport, csref;
	unsigned short csport, sport;

	void incomingMessage(Message);
	void monitor(void);
	
public:
	//port, csip, ident
	Server(unsigned short, string, unsigned short, string, unsigned int,
			string , string);
	// Must be format (username password);
	void addManager(string);

	//Main control loop
	//Returns when the server should exit
	void start(void);
	
	void stop(void);	
};
#endif
