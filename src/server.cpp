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
	FD_ZERO(&rsd);
	FD_SET(connection->sd, &rsd);	
	time(&tstart);
	while(1) {
		switch(select((connection->sd)+1, &rsd, NULL, NULL, &tv)) { 
			case -1:
				perror("select():");
				break;
			case 0:
				this->monitor();	
				time(&tstart);
				tv.tv_sec = 2;
				tv.tv_usec = 0;
				FD_ZERO(&rsd);
				FD_SET(connection->sd, &rsd);	
				break;
			default:
				time(&tend);
				if (tend - tstart > 2) {
					this->monitor();
					time(&tstart);
				}
				tv.tv_sec = 2;
				tv.tv_usec = 0;
				FD_ZERO(&rsd);
				FD_SET(connection->sd, &rsd);	
				message = connection->listen();
				this->incomingMessage(message);
				break;
		}
	}
}
	
void Server::stop(void){}

void Server::monitor(void) {
	entry_t * entries, entry, entry1;
	Message message;
	int size;
	entries = database->allEntries(SERVER, &size);
	message.setType(140);
	message.setMessage(ident);
	for(int i = 0 ; i < size ; i++) {
		message.setRecipients(*entries[i].name, ONE);
		connection->send(message);
		(*entries[i].pingtimeout)--;
		if(*entries[i].pingtimeout <=0) {
			//Also send 604 i believe?
			entry = entries[i];
			database->delete_(*entry.name);
			while(database->lookupIclient(entry.ip, entry.port, &entry1))
				database->delete_(*entry1.name);
		}
	}
	entries = database->allEntries(DCLIENT, &size);
	for(int i = 0 ; i < size ; i++) {
		message.setRecipients(*entries[i].name, ONE);
		connection->send(message);
		(*entries[i].pingtimeout)--;
		if(*entries[i].pingtimeout <=0) {
			database->delete_(*entries[i].name);
		}
	}
}
