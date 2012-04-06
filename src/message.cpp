#include <message.h>

using namespace std;
 
Message::Message(void){}

void Message::setType(int){}
int Message::getType(void){return 1;}
void Message::setMessage(string){}
string Message::getMessage(void){return data;}
	

void Message::setReferenceNumber(int){}
int Message::getReferenceNumber(void){return 1;}
	

void Message::setRecipients(string){}
string Message::getRecipients(void){return data;}

string Message::getSender(void){return data;}
