#include <server.h>
#include <cstdio>
#include <cstdlib>

extern "C" {
#include <arpa/inet.h>
#include <sys/select.h>
}

using namespace std;

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
	while(1) {
		switch(select((connection->sd)+1, &rsd, NULL, NULL, &tv)) { 
			case -1:
				perror("select():");
				break;
			case 0:
				printf("No Data Received\n");
				break;
			default:
				tv.tv_sec = 2;
				tv.tv_usec = 0;
				message = connection->listen();
				this->incomingMessage(message);
				break;
		}
	}
}
	
void Server::stop(void){}
	
void Server::incomingMessage(Message  message) {

	int type, temp, temp1, size, ref;
	string buffer;
	char cbuffer[100];
	entry_t entry, * entries;

	type = message.getType();
	buffer = message.getMessage();
	ref = message.getReferenceNumber();
	message.getSender(&entry.ip, &entry.port);
	switch (type) {
		case 100:
			printf("100 - client -> server (Received)\n");
			entry.name = new string(buffer.substr(0, buffer.find_first_of(" ")));
			if (database->lookup(*entry.name, NULL)) {
				printf("510 - server -> client (Sent) - Registratie mislukt\n");
				message.setType(510);
				message.setReferenceNumber(0);
				message.setMessage("Name already exists");
				connection->send(message, entry.ip, entry.port);
				break;
			}
			printf("500 - server -> client (Sent) - Registratie gelukt\n");
			entry.directlyconnected = 1;
			entry.isClient = 1;
			database->conClients++;
			database->insertReplaceWithIp(entry);
			message.setType(500);
			message.setMessage("");
			message.setRecipients(*entry.name, ONE);
			connection->send(message);
			
			printf("110 - server -> All but client (Sent) - user added\n");
			message.setType(110);
			message.setRecipients(*entry.name, ALLBUTONECLIENT);
			sprintf(cbuffer, "%6d%6d%s", 1, 1, (*entry.name).c_str());
			buffer = cbuffer;
			message.setMessage(buffer);
			connection->send(message);
			
			printf("110 - server -> client (Sending) - user list\n");
			entries = database->allEntries(&size);
			for (int i = 0 ; i < size ; i++) {
					sprintf(cbuffer, "%6d%6d%s", i+1, size, (*entries[i].name).c_str());
					buffer = cbuffer;
					message.setMessage(buffer);
					message.setRecipients(*entry.name, ONE);
					connection->send(message);
			}			
			break;			
		case 110:
			printf("110 - server -> server (Received)\n");
			if (buffer.length() > 12) {
				entry.name = new string (buffer.substr(12));
				message.getSender(&entry.ip, &entry.port);
				entry.directlyconnected = 0;
				entry.isClient = 1;
				database->insertReplace(entry);
			}
			if ( parentip == entry.ip && parentport == entry.port ) {
				printf("110 - server -> server (Sending to clients and non-parent servers)\n");
				message.setRecipients(parentname, ALLBUTONESERVER);
				connection->send(message);
				
				if (!buffer.substr(0, 6).compare(buffer.substr(6, 6))) {
					printf("110 - server -> parent server (Sending)\n");
					entries = database->allEntries(&size);
					temp = 0, temp1 = 0;
					
					for (int i = 0 ; i < size ; i++) {
						if (!(entries[i].ip == parentip && entries[i].port == parentport))
							temp ++;
					}
					if (temp == 0) {
						sprintf(cbuffer, "%6d%6d", 0, 0);
						buffer = cbuffer;
						message.setMessage(buffer);
						message.setRecipients(parentname, ONE);
						connection->send(message);
					}
					for (int i = 0 ; i < size ; i++) {
						if (!(entries[i].ip == parentip && entries[i].port == parentport)) {
							if (entries[i].isClient) {
								sprintf(cbuffer, "%6d%6d%s", ++temp1, temp, (*entries[i].name).c_str());
								buffer = cbuffer;
								message.setMessage(buffer);
								message.setRecipients(parentname, ONE);
								connection->send(message);
							}
						}
					}
				}
			}
			break;
		case 120:
			printf("120 - client -> server (Received)\n");
			if (! database->lookup(entry.ip, entry.port, &entry)) {
				break;	
			}
			message.setMessage(*entry.name + " " + buffer);
			message.setType(130);
			message.setRecipients("#all", ALL);
			connection->send(message);
			database->conClients--;
			database->delete_(*entry.name);	
			printf("130 - server -> All (Sent)\n");
		case 130:
			printf("130 - server -> server (Received)\n");
			if (!database->lookup(entry.ip, entry.port, &entry))
				break;
			if (database->lookup(buffer.substr(0, buffer.find_first_of(' ')), NULL)) {
				message.setRecipients(*entry.name, ALLBUTONESERVER);
				connection->send(message);
				printf("130 - server -> All (Sent)\n");
				database->delete_(buffer);
			}
			break;
		case 140:
			//printf("140 - ping -> 150 - pong\n"); Clutters the logs
			message.setType(150);
			message.setReferenceNumber(0);
			if ( entry.ip == csip && entry.port == htons(2001))
				message.setReferenceNumber(csref++);
			connection->send(message, entry.ip, entry.port);
			break;
		case 160:
			printf("160 - client -> server (Received)\n");
			if (!database->lookup(entry.ip, entry.port, &entry))
				break;
			if (database->lookup(buffer, NULL)) {
				printf("530 - server -> client (Sent)\n");
				message.setType(530);
				message.setRecipients(*entry.name, ONE);
				message.setMessage("Name already taken");
				connection->send(message);
				break;
			}
			buffer.insert(0, " ").insert(0, *entry.name);
			entry.name = new string(message.getMessage());
			entry.directlyconnected = 1;
			entry.isClient = 1;
			database->insertReplaceWithIp(entry);
			printf("520 - server -> client (Sent)\n");
			message.setType(520);
			message.setRecipients(*entry.name, ONE);
			message.setMessage("");
			connection->send(message);
			printf("170 - server -> All (Sent)\n");
			message.setType(170);
			message.setRecipients(*entry.name, ALL);
			message.setMessage(buffer);
			connection->send(message);
			break;
		case 170:
			printf("170 - server -> server (Received)\n");
			if (!database->lookup(entry.ip, entry.port, &entry))
				break;
			if (!database->lookup(buffer.substr(0, temp = buffer.find_first_of(' ')), &entry))
				break;
			*entry.name = buffer.substr(temp+1, string::npos);
			*entry.ref = 0;
			message.setRecipients(*entry.name, ALLBUTONESERVER);
			connection->send(message);
			printf("170 - server -> All (Sent)\n");
			break;
		case 200:
			printf("200 - client -> server (Received)\n");
			message.setType(300);
			if (! database->lookup(entry.ip, entry.port, &entry)) {
				break;	
			}
			buffer.insert(0, " ").insert(0, *entry.name);
			message.setMessage(buffer);
			temp = buffer.find_first_of(' ')+1;
			if (buffer.substr(temp, buffer.find_first_of (' ', temp+1)-temp) == "#all") {
				printf("300 -  server -> ALL (Sending)\n");
				message.setRecipients("#all", ALL);
				connection->send(message);
				break;
			}
			printf("300 -  server -> server/client (Sending)\n");
			temp = buffer.find_first_of(' ')+1;
			message.setRecipients(buffer.substr(temp, buffer.find_first_of(' ', temp)-temp), ONE);
			connection->send(message);
			message.setRecipients(*entry.name, ONE);
			connection->send(message);
			break;				
		case 600:
			printf("600 - server -> server (Received)\n");
			
			//Insert Server child into database
			entry.name = new string(buffer);
			entry.directlyconnected = 1;
			entry.isClient = 0;
			database->insertReplaceWithIp(entry);
			
			printf("110 - server -> server (Sending to new child)\n");
			entries = database->allEntries(&size);
			message.setType(110);
			message.setRecipients(*entry.name, ONE);
			for (int i = 0 ; i < size ; i++) {
				if (entries[i].isClient) {
					sprintf(cbuffer, "%6d%6d%s", i+1, size, (*entries[i].name).c_str());
					buffer = cbuffer;
					message.setMessage(buffer);
					connection->send(message);
				}
			}
			break;
		case 602:
			printf("602 - Control server -> server (Received)\n");
			if (buffer.compare("none") == 0) {
				printf("602 - No parent received (Now root of network)\n");
				break;
			}
			
			//Insert future parent into database
			inet_pton(AF_INET,buffer.substr(0, (temp = buffer.find_first_of(':'))).c_str(), &entry.ip);
			parentip = entry.ip;
			entry.port = htons(atoi( buffer.substr( temp+1, buffer.find_first_of(':',temp+1) - temp -1 ).c_str() ));
			parentport = entry.port;
			temp = buffer.find_first_of(':',temp+1);
			entry.name = new string (buffer.substr(temp+1, string::npos));
			parentname = *entry.name;
			entry.directlyconnected = 1;
			entry.isClient = 0;
			database->insert(entry);
							
			printf("600 - Server -> Server: Trying to register in network (Sending)");
			fflush(stdout);
			message.setType(600);
			message.setMessage(ident);
			message.setRecipients(*entry.name, ONE);
			connection->send(message);
			break;

		default:
			break;		
	}
}	
