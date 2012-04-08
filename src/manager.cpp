#include <manager.h>

using namespace std;

void Manager::setName(string _name)
{
	name = _name;
}

void Manager::setPassword(string _password)
{
	password = _password;
}

string Manager::getName(void)
{
	return name;
}

string Manager::getPassword(void)
{
	return password;
}

int Manager::isOnline(void)
{
	return online;
}

//Sets isOnline to true
void Manager::setAdress(unsigned long _ip, unsigned short _port)
{
	online = 1;
	ip = _ip;
	port =_port;
}
void Manager::getAdress(unsigned long * _ip, unsigned short * _port)
{
	* _ip = ip;
	* _port = port;
}

void Manager::setOffline(void)
{
	online = 0;
}
