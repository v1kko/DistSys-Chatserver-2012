#include <connection.h>

Connection::Connection(){} //Constructor, Port must be set to 2001!
Connection::Connection(int){} //Constructor, set port to arg1
void Connection::setPort(int){}
	
void Connection::setDatabase(Database){}
	
	/*
		Also checks if the message is valid
		Blocking function
	*/	
Message Connection::listen(void){return message;}

void Connection::send(Message){}
void Connection::sendManager(Message){}
