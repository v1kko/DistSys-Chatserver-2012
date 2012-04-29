#include <server.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
#include <arpa/inet.h>
#include <sys/select.h>
}

using namespace std;

#include <incomingMessage.cpp>
Server::Server(unsigned short _port, string _csip, unsigned short _csport, string _ident)
{
	struct sockaddr_in sa;
	inet_pton(AF_INET, _csip.c_str(), &sa.sin_addr);
	csip = sa.sin_addr.s_addr;
	csport = htons(_csport);
	csref = 0;
	cstimeout = noparenttimeout = CSTIMEOUT;
	ident = _ident;
	parentname = "Undefined";
	parentip = parentport = 0;
	connection = new Connection (_port);
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
	int succes;
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
	
	connection->send(message, csip, csport);
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
				message = connection->listen(&succes);
				if (succes)
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
	//Check Control server and parent server
	if (parentname == "Undefined" || parentfirst) {
		noparenttimeout--;
	} else {
		noparenttimeout=CSTIMEOUT;
	}
	cstimeout--;
	if (cstimeout <= 0 || noparenttimeout <= 0) {
		csref = 0;
		cstimeout=CSTIMEOUT;
		noparenttimeout = CSTIMEOUT;
		parentname = "Undefined";
		message.setType(601);
		message.setReferenceNumber(csref++);
		message.setMessage(ident);
		connection->send(message, csip, csport);
		printf("601 - Server -> Control server: Request for parent Server\n");
	}

	//Check servers
	entries = database->allEntries(SERVER, &size);
	message.setType(140);
	message.setMessage(ident);
	for(int i = 0 ; i < size ; i++) {
		message.setRecipients(*entries[i].name, ONE);
		connection->send(message);
		(*entries[i].pingtimeout)--;
		if(*entries[i].pingtimeout <=0) {
			entry = entries[i];
			//send 603 because a server is down
			printf("603 - Server -> Control server: Server is down\n");
			message.setType(603);
			message.setReferenceNumber(csref++);
			message.setMessage(*entry.name);
			database->delete_(*entry.name);
			connection->send(message, csip, csport);
			//Remove children of dead server
			while(database->lookupIclient(entry.ip, entry.port, &entry1)) {
				message.setType(130);
				message.setRecipients(*entry1.name, ALL);
				message.setMessage(*entry1.name + " Ping Timeout");
				database->delete_(*entry1.name);
				connection->send(message);
			}
		}
	}

	//Check DClients
	entries = database->allEntries(DCLIENT, &size);
	for(int i = 0 ; i < size ; i++) {
		message.setRecipients(*entries[i].name, ONE);
		connection->send(message);
		(*entries[i].pingtimeout)--;
		if(*entries[i].pingtimeout <=0) {
			message.setType(130);
			message.setRecipients(*entries[i].name, ALL);
			message.setMessage(*entries[i].name + " Ping Timeout");
			database->delete_(*entries[i].name);
			connection->send(message);
		}
	}
}
