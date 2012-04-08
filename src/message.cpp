#include <message.h>

using namespace std;
 
Message::Message(void){
	mtype = NONE;
	mdata = "";
	msize = 0;
	mrefnum = 0;
	mip = 0;
	mport = 0;
	mrecipient = "";
	mrecvtype = 0;
}

void Message::setType(int type)
{
	mtype = type;
}
int Message::getType(void)
{
	return mtype;
}

void Message::setMessage(string data)
{
	mdata = data;
	
	if (string::npos != 
		mdata.find_first_not_of("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ`~!@#$%^&*()-_=+[{]};:'\"\\|,<.>/?"))
		//Print warning here, be robust, keep the wheels turning!
	
	if (mdata.length()+6 > 200)
		mdata.resize(194);
	msize = mdata.length()+6;
}

string Message::getMessage(void)
{
	return mdata;
}

int Message::length(void)
{
return msize;
}

void Message::setReferenceNumber(int refnum)
{
	mrefnum = refnum;
}

int Message::getReferenceNumber(void)
{
	return mrefnum;
}
	
void Message::setRecipients(string recipient, int type)
{
	mrecipient = recipient;
	mrecvtype = type;
}

string Message::getRecipients(int * recvtype)
{
	* recvtype = mrecvtype;
	return mrecipient;
}

void Message::getSender(unsigned long * ip, unsigned short * port )
{
	* ip = mip;
	* port = mport;
}

void Message::setSender(unsigned long ip, unsigned short port)
{
	mip = ip;
	mport = port;
}
