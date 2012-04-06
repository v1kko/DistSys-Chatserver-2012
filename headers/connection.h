#ifndef _CONNECTION
#define _CONNECTION 1

#include <message.h>
#include <manager.h>
#include <database.h>
class Connection
{
private:
	Message message;
	Manager manager;

public:
	Connection(); //Constructor, Port must be set to 2001!
	Connection(int); //Constructor, set port to arg1
	void setPort(int);
	
	void setDatabase(Database);
	
	/*
		Also checks if the message is valid
		Blocking function
	*/	
	Message listen(void);

	void send(Message);
	void sendManager(Message);
};
#endif
