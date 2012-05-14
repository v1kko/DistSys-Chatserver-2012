#ifndef _MANAGER
#define _MANAGER 1

#include <string>
using namespace std;

class Manager {
private:
	string name, password;
	int online;
	unsigned long ip;
	unsigned short port;
	int reference;
public:
	int pingtimeout;
	void setName(string);
	void setPassword(string);
	string getName(void);
	string getPassword(void);
	
	void setRef(void);	
	int getRef(void);
	
	int isOnline(void);
	int isManager(string, string);	
	//Sets isOnline to true
	void setAdress(unsigned long, unsigned short);
	void getAdress(unsigned long *, unsigned short *);
	
	void setOffline(void);

};
#endif
