#ifndef _CONNECTION
#define _CONNECTION 1

#include <message.h>
#include <database.h>
class Connection
{
private:
	char * buffer;
	Database * database;
	int sd;

public:
	Connection(); //Constructor, Port must be set to 2001!
	Connection(int); //Constructor, set port to arg1
	
	void setDatabase(Database);
	
	/*
		Also checks if the message is valid
		(except the type number and referencenumber)
		Blocking function
	*/	
	Message listen(void);

	//send to receivers
	void send(Message);
	
	//send to specific ip adress
	//Also use for managmentmessages
	void send(Message, unsigned long, unsigned short);
};
#endif
