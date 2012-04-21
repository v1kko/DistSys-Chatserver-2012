#include <server.h>
#include <cstdio>
#include <cstdlib>

extern "C" {
#include <arpa/inet.h>
#include <sys/select.h>
}

using namespace std;

#include <incomingMessage.cpp>
Server::Server(unsigned short _port, unsigned long _csip, string _ident)
{
	port = _port;
	csip = _csip;
	csref = 0;
	ident = _ident;
	connection = new Connection (port);
	database = new Database();
	manager = new Manager();
	connection->setDatabase(database);
	database->conClients = 0;
}
	// Must be format (username password);
void Server::addManager(string data)
{
	manager->setPassword(data.substr(0, data.find_first_of (' ')));
	manager->setName(data.substr(data.find_first_of (' ')+1, string::npos));
}

	//Main control loop
	//Returns when the server should exit
void Server::start(void)
{
	struct timeval tv;
	time_t tstart, tend;
	fd_set rsd;
	FD_ZERO(&rsd);
	FD_SET(connection->sd, &rsd);	
	printf("Starting up: Our Tag is %s\n", ident.c_str());
	Message message;
	//unsigned long ip;
	//unsigned short _port;
	
	message.setType(601);
	message.setReferenceNumber(csref++);
	message.setMessage(ident);
	
	connection->send(message, csip, htons(2001));
	printf("601 - Server -> Control server: Request for parent Server\n");
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	time(&tstart);
	while(1) {
		switch(select((connection->sd)+1, &rsd, NULL, NULL, &tv)) { 
			case -1:
				perror("select():");
				break;
			case 0:
				this->monitor();	
				time(&tstart);
				break;
			default:
				time(&tend);
				if (tend - tstart > 2) {
					this->monitor();
					time(&tstart);
				}
				tv.tv_sec = 2;
				tv.tv_usec = 0;
				message = connection->listen();
				this->incomingMessage(message);
				break;
		}
	}
}
	
void Server::stop(void){}

void Server::monitor(void) {
	server * list;
	return_list(list,SERVER);
}
