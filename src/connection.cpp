#include <connection.h>
#include <string>
#include <cstdlib>
extern "C" 
{
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
}

using namespace std;

Connection::Connection(int port = 2001)
{
	buffer = (char *)malloc(sizeof(char)*201);
	int yes = 1;
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,(char*)&yes,sizeof(int));
	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	inet_pton(AF_INET, "0.0.0.0", &sa.sin_addr);
	bind(sd, (struct sockaddr *)&sa, sizeof(struct sockaddr_in));
	
}
	
void Connection::setDatabase(Database db)
{
	database = &db;
}

Message Connection::listen(void){
	Message message;
	struct sockaddr_in sender;
	socklen_t fromlen = sizeof(struct sockaddr_in);

	while (1) {
		uint16_t rlength = recvfrom(sd, &buffer, sizeof(char)*200, 0, (struct sockaddr *)&sender, &fromlen);
		if (rlength < 6)
			continue;
		
		uint16_t length = ntohs((uint16_t)buffer[0]);
		uint16_t type = ntohs((uint16_t)buffer[2]);
		uint16_t refnum = ntohs((uint16_t)buffer[4]);

		if (rlength != length)
			continue;
		
		buffer[length] = '\0';
		string t = &buffer[6];
		if (t.length() + 6 != length)
			continue;
		if (string::npos != 
		t.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ`~!@#$%^&*()-_=+[{]};:'\"\\|,<.>/?"))
			continue;
		
		message.setType(type);
		message.setReferenceNumber(refnum);
		message.setMessage(t);
		message.setSender(sender.sin_addr.s_addr, sender.sin_port);
		
	}
	return message;
}

void Connection::send(Message message)
{
	int sendtype, size;
	string recipient = message.getRecipients(&sendtype);
	entry_t entry, * entries;
	
	switch(sendtype) {
		case NONE:
			break;
		case ONE:
			if (database->lookup(recipient, &entry) != 0)
				this->send(message, entry.ip, entry.port);
			break;
		case ALL:
			entries  = database->allEntries(&size);
			for (int i = 0 ; i < size ; i++)
				this->send(message, entries[i].ip, entries[i].port);
			break;
		case ALLBUTONE:
			entries  = database->allEntries(&size);
			for (int i = 0 ; i < size ; i++) {
				if (entries[i].name->compare(recipient) != 0)
					this->send(message, entries[i].ip, entries[i].port);
			}
			break;
		default:
			break;
	}
}

void Connection::send(Message message, unsigned long ip, unsigned short port)
{
	struct sockaddr_in destination;
	destination.sin_addr.s_addr = ip;
	destination.sin_port = port;
	
	uint16_t length = message.length();
	uint16_t type = message.getType();
	uint16_t refnum = message.getReferenceNumber();
	
	buffer[0] = htons(length);
	buffer[2] = htons(type);
	buffer[4] = htons(refnum);
	strcpy(&buffer[6], message.getMessage().c_str());
	
	sendto(sd, buffer, length, 0, (struct sockaddr *)&destination, sizeof(struct sockaddr_in));
}
